#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>

#include "../console/console.hpp"
#include "../global/global.hpp"

namespace invoker {
#pragma pack(push, 8)
	class internal_native_call_ctx_t
	{
	public:
		void reset()
		{
			m_arg_count = 0;
			m_data_count = 0;
		}

		template<typename T>
		void push_arg(T&& value)
		{
			static_assert(sizeof(T) <= sizeof(std::uint64_t));
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(reinterpret_cast<std::uint64_t*>(m_args) + (m_arg_count++)) = std::forward<T>(value);
		}

		template<typename T>
		T& get_arg(std::size_t index)
		{
			static_assert(sizeof(T) <= sizeof(std::uint64_t));
			return *reinterpret_cast<T*>(reinterpret_cast<std::uint64_t*>(m_args) + index);
		}

		template<typename T>
		void set_arg(std::size_t index, T&& value)
		{
			static_assert(sizeof(T) <= sizeof(std::uint64_t));
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(reinterpret_cast<std::uint64_t*>(m_args) + index) = std::forward<T>(value);
		}

		template<typename T>
		T* get_return_value() {
			return reinterpret_cast<T*>(m_return_value);
		}

		template<typename T>
		void set_return_value(T&& value)
		{
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(m_return_value) = std::forward<T>(value);
		}

		template<typename T>
		void set_return_value(T& value)
		{
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(m_return_value) = std::forward<T>(value);
		}

	protected:
		void* m_return_value{};
		std::uint32_t m_arg_count{};
		void* m_args{};
		std::int32_t m_data_count{};
		std::uint32_t m_data[48]{};
	};
	static_assert(sizeof(internal_native_call_ctx_t) == 0xE0);

	class native_call_ctx_t : public internal_native_call_ctx_t
	{
	public:
		native_call_ctx_t()
		{
			m_return_value = &m_return_stack[0];
			m_args = &m_arg_stack[0];
		}
	private:
		std::uint64_t m_return_stack[10]{ 0 };
		std::uint64_t m_arg_stack[100]{ 0 };
	};
#pragma pack(pop)


	using raw_decrypt_native_t = std::uint64_t(__fastcall*)(std::uint64_t list, std::uint64_t hash);
	using native_conv_t = void(__fastcall*)(native_call_ctx_t* ctx);

	/* 
	The native resolver is a function with takes the native tableand a native hashand resolves the hash to the corresponding handler, returning
	the handler's address as a uintptr_t. The signature for all handlers is void(__fastcall*)(native_call_ctx_t*), meaning they all return void and use the native call
	context for retrieving arguments and returning values.
	*/

	constexpr auto RESOLVE_HANDLERS = false;
	template<typename RetT, std::intptr_t NativeHash, std::uintptr_t NativeHandler, typename... Args>
	RetT FORCEINLINE invoke(Args&&... args) {
		native_conv_t native_handler{};
		if constexpr(RESOLVE_HANDLERS) {
			const auto native_resolver{ global::base + global::native_resolver::native_resolver_rva };
			const auto native_table{ global::base + global::native_resolver::native_table_rva };
			const auto native_handler_addr = reinterpret_cast<invoker::raw_decrypt_native_t>(native_resolver)(native_table, NativeHash);

			if (!native_handler_addr) {
				console::log<log_severity::error>("Failed to resolve native hash: %llX", NativeHash);
				return RetT{};
			}

			native_handler = reinterpret_cast<invoker::native_conv_t>(native_handler_addr);
		} else
			native_handler = reinterpret_cast<invoker::native_conv_t>(global::base + NativeHandler);

		invoker::native_call_ctx_t ctx{};
		(ctx.push_arg(std::forward<Args>(args)), ...);

		native_handler(&ctx);
		if constexpr (!std::is_same_v<void, RetT>)
		{
			if (const auto ret_val = ctx.get_return_value<RetT>(); ret_val != nullptr)
				return *ret_val;
			console::log<log_severity::error>("Return value returned from native handler is `nullptr`");
			return RetT{};
		}
	}
}
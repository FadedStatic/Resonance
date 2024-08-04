#pragma once
#include <shared_mutex>
#include <atomic>

namespace global {
	const auto base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));

	namespace menu {
		inline std::atomic_bool menu_open{ false };
		inline std::atomic_bool menu_exit;
	}

	namespace native_resolver {
		constexpr auto native_resolver_rva = 0x1633EF8;
		constexpr auto native_table_rva = 0x2F22540;
		constexpr auto sm_threads = 0x2f24630;
	}

	template<typename _Ty>
	struct shared_var {
		std::shared_mutex obj_mutex;
		_Ty object;

		_Ty get() {
			std::shared_lock<std::shared_mutex> lock(this->obj_mutex);
			return object;
		}

		void set(const _Ty obj) {
			std::unique_lock<std::shared_mutex> lock(this->obj_mutex);
			object = obj;
		}

		void operator=(const _Ty obj) {
			set(obj);
		}

		operator _Ty() {
			return get();
		}

		shared_var(_Ty obj) : object(obj) {}
	};

}
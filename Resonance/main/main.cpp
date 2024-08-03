#include <iostream>
#include <thread>
#include <Windows.h>
#include "../hooking/include.hpp"
#include "../invoker.hpp"

using scr_thread_run_t = void(__thiscall*)(void*);

std::uintptr_t* orig_scr_thread_run_addr{ nullptr };
std::once_flag flag;

#pragma pack(push, 1)
class scr_vector_t
{
public:
	scr_vector_t() = default;

	scr_vector_t(float x, float y, float z) :
		x(x), y(y), z(z)
	{}
public:
	float x{};
private:
	char m_padding1[0x04];
public:
	float y{};
private:
	char m_padding2[0x04];
public:
	float z{};
private:
	char m_padding3[0x04];
};

void __stdcall callback(void* scr_thread_obj) {
	std::call_once(flag, [&] {

		const auto ped = invoker::invoke<int, 0x56E414973C2A8C0E>(-1);
		scr_vector_t coords = invoker::invoke<scr_vector_t, 0xD1A6A821F5AC81DB>(ped, true);

		console::log<log_severity::info>("Player entity coordinates: { X: %f, Y: %f, Z: %f } ", coords.x, coords.y, coords.z);

		reinterpret_cast<scr_thread_run_t>(orig_scr_thread_run_addr)(scr_thread_obj);
	});
}


void main(HMODULE dll)
{
	process main_proc{};
	console console{};

	const auto base_addr = get_base_address();

	console.log<log_severity::info>("Base address: %llX", base_addr);
	const auto threads = at_array_t<scrThread*>(reinterpret_cast<void*>(base_addr + sm_threads));

	scrThread* persistent_thread{ nullptr };
	
	for (const auto& thread : threads)
	{
		if (thread->m_script_hash == 0x5700179C) {
			console::log<log_severity::success>("Main Persistent Thread Hash: %X, Thread ID: %X, scrThread address: %p", thread->m_script_hash, thread->m_thread_id, thread);
			persistent_thread = thread;
			break;
		}
		console::log<log_severity::info>("Thread hash: %X", thread->m_script_hash);
	}

	hk_scr_thread_run_t hk{ persistent_thread, reinterpret_cast<std::uintptr_t*>(&callback), orig_scr_thread_run_addr};
	std::this_thread::sleep_for(std::chrono::seconds(5));
	FreeLibrary(dll);
}

int __stdcall DllMain(HMODULE calling_module, unsigned long reason_for_call, void* reserved)
{
	switch (reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		std::thread{ main, calling_module }.detach();
		break;
	}

	return 1;
}
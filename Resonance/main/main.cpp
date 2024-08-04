#include <iostream>
#include <thread>
#include <Windows.h>
#include "../hooking/include.hpp"
#include "../update/natives.hpp"

using scr_thread_run_t = void(__thiscall*)(void*);

std::uintptr_t* orig_scr_thread_run_addr{ nullptr };
std::once_flag flag;

void __stdcall callback(void* rcx) {
	std::call_once(flag, [&] {

		const auto ped = PLAYER::GET_PLAYER_PED(-1);
		const auto coords = ENTITY::GET_ENTITY_COORDS(ped, true);

		console::log<log_severity::info>("Player entity coordinates: { X: %f, Y: %f, Z: %f } ", coords.x, coords.y, coords.z);
		reinterpret_cast<scr_thread_run_t>(orig_scr_thread_run_addr)(rcx);
	});
}


void main(HMODULE dll)
{
	process main_proc{};
	console console{};

	const auto base_addr = get_base_address();

	console::log<log_severity::info>("Base address: %llX", base_addr);
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
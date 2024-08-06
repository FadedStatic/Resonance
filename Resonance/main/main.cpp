#include <iostream>
#include <thread>
#include <Windows.h>
#include "../hooking/hooking.hpp"
#include "../menu/menu.hpp"

menu main_menu;
using scr_thread_run_t = std::uint32_t(__thiscall*)(void* self, int ops);

std::uintptr_t* orig_scr_thread_run_addr{ nullptr };
std::once_flag flag;

std::uint32_t  __stdcall callback(void* _this, int ops) {
	main_menu.render();

	if (GetAsyncKeyState(VK_END) & 0x8000)
		global::menu::menu_exit = true;

	return reinterpret_cast<scr_thread_run_t>(orig_scr_thread_run_addr)(_this, ops);
}


void main(HMODULE dll)
{
	console console{};

	console::log<log_severity::info>("Base address: %llX", global::base);
	const auto threads = at_array_t<scrThread*>(reinterpret_cast<void*>(global::base + global::native_resolver::sm_threads));

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
	while(!global::menu::menu_exit) {}
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
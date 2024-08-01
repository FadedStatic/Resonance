#include <iostream>
#include <thread>
#include <Windows.h>
#include "../hooking/include.hpp"

using scr_thread_run_t = void(__thiscall*)(void*);

std::uintptr_t* p_scr_thread_run_buf{ nullptr };

void __stdcall callback(void* scr_thread_obj) {
	console::log<console::log_severity::success>("Calling from `callback`");
	reinterpret_cast<scr_thread_run_t>(p_scr_thread_run_buf)(scr_thread_obj);
}


void main(HMODULE dll)
{
	process main_proc = process();
	console::init();
	const auto base_addr = get_base_address();

	console::log<console::log_severity::info>("Base address: %X", base_addr);
	const auto threads = at_array_t<scrThread*>(reinterpret_cast<void*>(base_addr + sm_threads));

	scrThread* persistent_thread{ nullptr };
	
	for (const auto& thread : threads)
	{
		if (thread->m_script_hash == 0x5700179C) {
			console::log<console::log_severity::success>("Main Persistent Thread Hash: %X, Thread ID: %X, scrThread address: %p", thread->m_script_hash, thread->m_thread_id, thread);
			persistent_thread = thread;
			break;
		}
		console::log<console::log_severity::info>("Thread hash: %X", thread->m_script_hash);
	}

	const auto vtable = *reinterpret_cast<std::uintptr_t***>(persistent_thread);
	const auto p_scr_thread_run = &vtable[2];

	p_scr_thread_run_buf = *p_scr_thread_run;	
	auto oldpf{ 0ul };
	VirtualProtect(p_scr_thread_run, sizeof std::size_t, PAGE_EXECUTE_READWRITE, &oldpf);

	console::log<console::log_severity::warn>("Overwriting scrThread::Run virtual function table entry with callback address: [%p]", &callback);
	*p_scr_thread_run = reinterpret_cast<std::uintptr_t*>(&callback);

	console::log<console::log_severity::success>("Successfully overwrote virtual function table entry at: [%p]", p_scr_thread_run);

	std::this_thread::sleep_for(std::chrono::seconds(10));

	console::log<console::log_severity::warn>("Restoring virtual function table entry");
	*p_scr_thread_run = p_scr_thread_run_buf;

	VirtualProtect(p_scr_thread_run, sizeof std::size_t, oldpf, &oldpf);
	console::log<console::log_severity::success>("Restored!");

	FreeLibrary(dll);
	FreeConsole();
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
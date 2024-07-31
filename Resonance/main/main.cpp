#include <iostream>
#include <thread>
#include <Windows.h>
#include "../hooking/include.hpp"

int main()
{
	process main_proc = process();
	console::init();
	const auto threads = at_array_t<void*>((void*)(get_base_address() + sm_threads));
	for (const auto& thread : threads)
	{
		console::log<console::log_severity::info>("Thread: %llX", thread);
	}
	return 0;
}

int __stdcall DllMain(HINSTANCE__* calling_module, unsigned long reason_for_call, void* reserved)
{
	switch (reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		std::thread{ main }.detach();
		break;
	}

	return 1;
}
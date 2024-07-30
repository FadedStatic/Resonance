#include "include.hpp"

namespace hooking {
	using scr_thread_updateall_t = std::int64_t(*)(std::int32_t);
	scr_thread_updateall_t original_scr_thread_updateall;
	void init(process& proc)
	{
		MH_Initialize();
		const auto thing = scanner::scan(proc, "\xB8\x40\x5D\xC6\x00\x45", "xxxxxx");

		console::log<console::log_severity::success>("Found at: %02llX (%d results)", thing.at(0).loc - (uintptr_t)GetModuleHandleA(nullptr), thing.size());

		original_scr_thread_updateall = reinterpret_cast<scr_thread_updateall_t>(thing.at(0).loc);
		MH_CreateHook(original_scr_thread_updateall, scr_thread_updateall, reinterpret_cast<void**>(&original_scr_thread_updateall));
		MH_EnableHook(original_scr_thread_updateall);
	}
}
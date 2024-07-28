#pragma once

#include <cstdint>
#include "../libs/minhook/MinHook.hpp"
#include "../console/include.hpp"
#include "../libs/scanner/scanner.hpp"

namespace hooking {
	void init(process& proc);

	std::int64_t scr_thread_updateall(std::int32_t insncount);
}
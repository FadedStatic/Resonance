#pragma once

#include <cstdint>
#include "../libs/minhook/MinHook.hpp"
#include "../console/include.hpp"
#include "../libs/scanner/scanner.hpp"

namespace hooking {
	void init(process& proc);
}
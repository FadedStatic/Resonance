#include "include.hpp"

namespace hooking {
	using scr_thread_updateall_t = std::int64_t(*)(std::int32_t);
	scr_thread_updateall_t original_scr_thread_updateall;
	void init(process& proc)
	{

	}
}
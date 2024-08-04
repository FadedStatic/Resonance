#pragma once

#include <cstdint>
#include "../console/include.hpp"
#include "../libs/scanner/scanner.hpp"
#include "../update/include.hpp"


struct hk_scr_thread_run_t {
	scrThread* thread;
	std::uintptr_t** scr_thread_run_ptr_addr;
	std::uintptr_t* orig_scr_thread_run_addr;

	hk_scr_thread_run_t(scrThread* _thread, std::uintptr_t* callback, std::uintptr_t*& scr_thread_run_buf) noexcept : thread{ _thread } {
		const auto vtable = *reinterpret_cast<std::uintptr_t***>(this->thread);
		
		this->scr_thread_run_ptr_addr = &vtable[3];
		this->orig_scr_thread_run_addr = *scr_thread_run_ptr_addr;
		scr_thread_run_buf = *scr_thread_run_ptr_addr;
		

		console::log<log_severity::warn>("Overwriting scrThread::Run virtual function table entry with callback address: [%p]", &callback);
		*scr_thread_run_ptr_addr = callback;

		console::log<log_severity::success>("Successfully overwrote virtual function table entry at: [%p]", scr_thread_run_ptr_addr);

	}

	~hk_scr_thread_run_t() {

		console::log<log_severity::warn>("Restoring virtual function table entry");
		*scr_thread_run_ptr_addr = orig_scr_thread_run_addr;
		console::log<log_severity::success>("Restored!");
	}
};
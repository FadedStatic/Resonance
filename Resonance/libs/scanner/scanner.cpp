/*
	DWYW License (Do whatever you want)

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software, to do whatever they want to do with this software.

	This is not copyrighted, and you don't need to worry about licensing issues.

	In no event shall the authors be liable for any claim,
	damages, or other liability, whether in an action of contract,
	tort or otherwise, arising from, out of or in connection with
	the software or the use or other dealings in the software.
*/

#include "scanner.hpp"

#if PERFORMANCE_PROFILING_MODE == true
	#include <chrono>
#endif

process::process() {
	IsWow64Process(GetCurrentProcess, reinterpret_cast<BOOL*>(&this->is32));
	this->is32 = !!this->is32;
	this->curr_proc = GetCurrentProcess();
	this->curr_mod = GetModuleHandleA(nullptr);
	this->proc_base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
	this->pid = GetCurrentProcessId();
}

process::process(const std::string_view process_name) {
	std::vector<std::uint32_t> pid_list(max_processes);
	std::vector<HMODULE> module_list(max_modules);
	DWORD n_pids{ 0 };
	K32EnumProcesses(reinterpret_cast<DWORD*>(pid_list.data()), static_cast<std::uint32_t>(pid_list.capacity()) * sizeof(DWORD), &n_pids);

	pid_list.resize(n_pids / 4);
	std::string module_name(MAX_PATH, '\x0');

	for (const auto i : pid_list) {
		const auto proc_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, i);
		DWORD n_modules{ 0 };

		if (!proc_handle or !K32EnumProcessModulesEx(proc_handle, module_list.data(), static_cast<std::uint32_t>(module_list.capacity()) * sizeof(HMODULE), &n_modules, LIST_MODULES_ALL) or !K32GetModuleBaseNameA(proc_handle, nullptr, module_name.data(), MAX_PATH))
			continue;

		std::erase_if(module_name, [](const char c) {
			return !c;
		});

		if (module_name != process_name) {
			module_name.clear();
			module_name.resize(MAX_PATH);
			continue;
		}

		module_list.resize(n_modules / sizeof(HMODULE));
		for (const auto& j : module_list) {
			if (!K32GetModuleBaseNameA(proc_handle, j, module_name.data(), MAX_PATH))
				continue;

			std::erase_if(module_name, [](const char c) {
				return !c;
			});

			if (module_name == process_name.data()) {
				this->proc_base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
				if (i != GetCurrentProcessId()) {
					MODULEINFO mod_info;
					if (K32GetModuleInformation(proc_handle, j, &mod_info, sizeof(mod_info)))
						this->proc_base = reinterpret_cast<std::uintptr_t>(mod_info.lpBaseOfDll);
				}

				IsWow64Process(proc_handle, reinterpret_cast<BOOL*>(&this->is32));
				this->is32 = !!this->is32;
				this->curr_proc = proc_handle;
				this->pid = i;
				this->curr_mod = j;
				return;
			}

			module_name.clear();
			module_name.resize(MAX_PATH);
		}

		module_list.clear();
		module_list.resize(max_modules);
	}

	this->curr_proc = nullptr;
	this->proc_base = 0;
	this->curr_mod = nullptr;
	this->is32 = false;
	this->pid = 0;
}

std::vector<scan_result> scanner::scan(const process& proc, const std::string_view aob, const std::string_view mask, const scan_cfg& config, const scanner_optargs& opt_args) {
#if SET_PRIORITY_OPTIMIZATION == true
	const auto old_priority = GetPriorityClass(GetCurrentProcess());
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
#endif

#if PERFORMANCE_PROFILING_MODE == true
	const auto start_time = std::chrono::high_resolution_clock::now();
#endif

	std::vector<std::thread> thread_list;

	std::shared_mutex ret_lock;
	std::vector<scan_result> ret;

	const auto is_internal = GetCurrentProcessId() == proc.pid;
	const auto is_modulerange = !config.module_scanned.empty();
	auto mod_found = proc.curr_mod;

	auto scan_base_address = reinterpret_cast<std::uintptr_t>(is_modulerange and is_internal ? GetModuleHandleA(config.module_scanned.data()) : GetModuleHandleA(nullptr));

	if (!is_internal ) {
		if (is_modulerange) {
			std::vector<HMODULE> module_list(max_modules);
			DWORD n_modules{ 0 };
			std::string module_name(MAX_PATH, '\x0');

			if (K32EnumProcessModulesEx(proc.curr_proc, module_list.data(), static_cast<std::uint32_t>(module_list.capacity()) * sizeof(HMODULE), &n_modules, LIST_MODULES_ALL)) {
				module_list.resize(n_modules / sizeof(HMODULE));
				for (const auto& j : module_list) {
					if (K32GetModuleBaseNameA(proc.curr_proc, j, module_name.data(), MAX_PATH)) {
						std::erase_if(module_name, [](const char c) {
							return !c;
						});

						if (module_name == config.module_scanned)
							mod_found = j;
					}

					module_name.clear();
					module_name.resize(MAX_PATH);
				}
			}
		}

		MODULEINFO mod_info;
		if (K32GetModuleInformation(proc.curr_proc, mod_found, &mod_info, sizeof(mod_info)))
			scan_base_address = reinterpret_cast<std::uintptr_t>(mod_info.lpBaseOfDll);
	}

	auto scan_end_address = ~0ull;
	if (is_modulerange) {
		MODULEINFO mod_info;
		if (K32GetModuleInformation(proc.curr_proc, is_internal ? GetModuleHandleA(config.module_scanned.data()) : mod_found, &mod_info, sizeof(mod_info)))
			scan_end_address = scan_base_address + mod_info.SizeOfImage;
	}

	if (!mod_found)
		return ret;

	MEMORY_BASIC_INFORMATION mbi;

	for (auto scan_address = scan_base_address; scan_address < scan_end_address; scan_address += 16) {
		// Credits to Fishy.
		if (is_internal ? !VirtualQuery(reinterpret_cast<LPCVOID>(scan_address), &mbi, sizeof(MEMORY_BASIC_INFORMATION)) : !VirtualQueryEx(proc.curr_proc, reinterpret_cast<LPCVOID>(scan_address), &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
			break;

		if (config.page_flag_check(mbi)) {
			std::thread analyze_page(is_internal ? std::ref(config.scan_routine_internal) : std::ref(config.scan_routine_external),
				scanner_args {
					std::ref(proc),
					reinterpret_cast<std::uintptr_t>(mbi.BaseAddress),
					reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + static_cast<std::uintptr_t>(mbi.RegionSize),
					std::ref(ret_lock), std::ref(ret),
					std::ref(aob), std::ref(mask),
					std::ref(opt_args)
				});
			thread_list.push_back(std::move(analyze_page));
		}

		scan_address += mbi.RegionSize;
	}

	for (auto& thread : thread_list)
		thread.join();

#if SET_PRIORITY_OPTIMIZATION == true
	SetPriorityClass(GetCurrentProcess(), old_priority);
#endif

#if PERFORMANCE_PROFILING_MODE == true
	const auto end_time = std::chrono::high_resolution_clock::now();
	std::printf("Time taken: %lldms\n", std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count());
#endif
	return ret;
}

void scanner_cfg_templates::aob_scan_routine_external_default(const scanner_args& args) {
	const auto& [proc, start, end, return_vector_mutex, return_vector, aob, mask, ignore1] = args;
	const auto page_size = end - start;
	std::size_t n_read;
	std::vector<std::uint8_t> page_memory(page_size);

	std::vector<scan_result> local_results;

	ReadProcessMemory(proc.curr_proc, reinterpret_cast<LPCVOID>(start), page_memory.data(), page_size, reinterpret_cast<SIZE_T*>(&n_read));

	for (std::uintptr_t i = 0ull; i < page_memory.size(); i++) {
		for (std::uintptr_t j = 0ull; j < mask.length(); j++)
			if (mask[j] != '?' and (page_memory[i + j] != static_cast<std::uint8_t>(aob[j])))
				goto out_of_scope;

		local_results.push_back({ i+start });

		// this is where that "optimization" started, this was the cleanest solution I could think of
		out_of_scope:
		continue;
	}

	if (!local_results.empty()) {
		return_vector_mutex.lock();
		for (const auto& c : local_results)
			return_vector.push_back(c);
		return_vector_mutex.unlock();
	}
}

void scanner_cfg_templates::aob_scan_routine_internal_default(const scanner_args& args) {
	const auto& [proc, start, end, return_vector_mutex, return_vector, aob, mask, ignore1] = args;
	std::vector<scan_result> local_results;

	for (const auto* i = reinterpret_cast<std::uint8_t*>(start); i < reinterpret_cast<std::uint8_t*>(end); i++) {
		for (auto j = 0ull; j < mask.length(); j++)
			if (mask[j] != '?' and *(i+j) != static_cast<std::uint8_t>(aob[j]))
				goto out_of_scope;

		local_results.push_back({ reinterpret_cast<std::uintptr_t>(i) });

		out_of_scope:
		continue;
	}

	if (!local_results.empty()) {
		return_vector_mutex.lock();
		for (const auto& c : local_results)
			return_vector.push_back(c);
		return_vector_mutex.unlock();
	}
}

std::vector<scan_result> scanner::string_scan(const process& proc, const std::string_view str, const scan_cfg& config, const std::uintptr_t n_result) {
	auto str_results = scanner::scan(proc, str, std::string("x", str.length()), config);

	if (str_results.empty())
		return str_results;

	auto str_loc_endianized = std::string(4, '\x0');

	if (proc.is32) {
		const auto loc = static_cast<std::uint32_t>(str_results[n_result].loc);
		std::memcpy(str_loc_endianized.data(), &loc, 4);
	}

	return scan(proc, "strscan", "xxxxxxx", { config.module_scanned, config.page_flag_check, config.min_page_size, config.max_page_size, scanner_cfg_templates::string_xref_scan_internal_default, scanner_cfg_templates::string_xref_scan_external_default }, { str_loc_endianized, str_results[n_result].loc });
}

void scanner_cfg_templates::string_xref_scan_external_default(const scanner_args& args) {
	const auto& [proc, start, end, return_vector_mutex, return_vector, aob, mask, optargs] = args;
	const auto xref_trace = optargs.xref_trace_endianized;
	const auto page_size = end - start;
	std::size_t n_read;
	std::vector<std::uint8_t> page_memory(page_size);

	// Chunking, instead of locking the mutex (very slow) just do it when we're done.
	std::vector<scan_result> local_results;

	ReadProcessMemory(proc.curr_proc, reinterpret_cast<LPCVOID>(start), page_memory.data(), page_size, reinterpret_cast<SIZE_T*>(&n_read));

	for (std::uintptr_t i = 0ull; i < page_memory.size(); i++) {
		switch (page_memory[i]) {
		// ABSOLUTE
		case 0xB9: // mov ecx, offset loc_01020304 -> B9 04 03 02 01
		case 0xBA: // mov edx, offset loc_01020304 -> BA 04 03 02 01
		case 0xB8: // mov eax, offset loc_01020304 -> B8 04 03 02 01
		case 0x68: // push offset loc_01020304 -> 68 04 03 02 01
			if (proc.is32 and i+5 < page_size and !std::memcmp(&xref_trace[0], &page_memory[i+1], 4))
				local_results.push_back({ start + i });
			break;
		case 0xC7: // mov [reg + off], loc_01020304 -> C7 ? ? 04 03 02 01
		//case 0x0F: // Twobyte, this can pessimize performance (twice as slow), so it is advised that you leave this off.
			if (proc.is32 and i+7 < page_size and !std::memcmp(&xref_trace[0], &page_memory[i+3], 4))
					local_results.push_back({ start + i });
				
			break;

		// RELATIVE
		case 0x48: // 64bit operation
			if (!proc.is32)
				switch (page_memory[i+1]) {
					case 0x8D: // LEA
						if (i + start + 7 + *reinterpret_cast<std::uint32_t*>(&page_memory[i+3]) == optargs.xref_trace_int)
							local_results.push_back({ start + i });

					break;
					default:break;
				}
		break;

		default:break;
		}
	}

	if (!local_results.empty()) {
		return_vector_mutex.lock();
		for (const auto& c : local_results)
			return_vector.push_back(c);
		return_vector_mutex.unlock();
	}
}

void scanner_cfg_templates::string_xref_scan_internal_default(const scanner_args& args) {
	const auto& [proc, start, end, return_vector_mutex, return_vector, aob, mask, optargs] = args;
	const auto xref_trace = optargs.xref_trace_endianized;

	std::vector<scan_result> local_results;

	for (const auto* i = reinterpret_cast<std::uint8_t*>(start); i < reinterpret_cast<std::uint8_t*>(end); i++) {
		switch (*i) {
			// ABSOLUTE
			case 0xB9: // mov ecx, offset loc_01020304 -> B9 04 03 02 01
			case 0xBA: // mov edx, offset loc_01020304 -> BA 04 03 02 01
			case 0xB8: // mov eax, offset loc_01020304 -> B8 04 03 02 01
			case 0x68: // push offset loc_01020304 -> 68 04 03 02 01
				if (proc.is32 and reinterpret_cast<std::uintptr_t>(i)+5 < end and !std::memcmp(&xref_trace[0], i+1, 4))
					local_results.push_back({ reinterpret_cast<std::uintptr_t>(i) });
			break;
			case 0xC7: // mov [reg + off], loc_01020304 -> C7 ? ? 04 03 02 01
				//case 0x0F: // Twobyte, this can pessimize performance (twice as slow), so it is advised that you leave this off.
				if (proc.is32 and reinterpret_cast<std::uintptr_t>(i)+7 < end and !std::memcmp(&xref_trace[0], i+1, 4))
					local_results.push_back({ reinterpret_cast<std::uintptr_t>(i) });

			break;

			// RELATIVE
			case 0x48: // 64bit operation
				if (!proc.is32)
					switch (*(i+1)) {
						case 0x8D: // LEA
							if (reinterpret_cast<std::uintptr_t>(i) + 7 + *reinterpret_cast<const std::uint32_t*>(i+1) == optargs.xref_trace_int)
								local_results.push_back({ reinterpret_cast<std::uintptr_t>(i) });

						break;
						default:break;
					}
			break;

			default:break;
		}
	}

	if (!local_results.empty()) {
		return_vector_mutex.lock();
		for (const auto& c : local_results)
			return_vector.push_back(c);
		return_vector_mutex.unlock();
	}
}

std::vector<scan_result> scanner::xref_scan(const process& proc, const std::uintptr_t func, const scan_cfg& config, const bool include_twobyte) {
	return scan(
		proc, 
		"xrefscan", 
		"xxxxxxxx", 
		
		{
			config.module_scanned,
			config.page_flag_check,
			config.min_page_size,
			config.max_page_size,
			scanner_cfg_templates::function_xref_scan_internal_default,
			scanner_cfg_templates::function_xref_scan_external_default
		},

		{
			[func] {
				std::vector<std::uint8_t> bytes_extracted(4);

				const auto loc = static_cast<std::uint32_t>(func);

				std::memcpy(bytes_extracted.data(), &loc, 4);

				std::string retnvec;
				for (const auto& thing : bytes_extracted)
					retnvec.push_back(static_cast<const char>(thing));
				
				return retnvec;
			}(),
			func
		});
}

void scanner_cfg_templates::function_xref_scan_external_default(const scanner_args& args) {
	const auto& [proc, start, end, return_vector_mutex, return_vector, aob, mask, optargs] = args;
	const auto xref_trace = optargs.xref_trace_endianized;
	const auto page_size = end - start;

	std::size_t n_read;
	std::vector<std::uint8_t> page_memory(page_size);

	std::vector<scan_result> local_results;

	ReadProcessMemory(proc.curr_proc, reinterpret_cast<LPCVOID>(start), page_memory.data(), page_size, reinterpret_cast<SIZE_T*>(&n_read));

	for (std::uintptr_t i = 0ull; i < page_memory.size(); i++) {
		switch (page_memory[i]) {
		// RELATIVE
		case 0xE9: // JMP Jz
		case 0xE8: // CALL Jz
			if (i + start + 5 + *reinterpret_cast<std::uint32_t*>(&page_memory[i+1]) == optargs.xref_trace_int)
				local_results.push_back({ start + i });
			break;
		case 0x48: // 64bit operation
			if (!proc.is32)
				switch (page_memory[i+1]) {
					case 0x8D:
						if (i + start + 7 + *reinterpret_cast<std::uint32_t*>(&page_memory[i+3]) == optargs.xref_trace_int)
							local_results.push_back({ start + i });

					break;
					default:break;
				}
		break;
		// ABSOLUTE
		case 0x68: // PUSH Av
		case 0x9A: // CALL Az
			if (proc.is32 and !std::memcmp(&page_memory[i+1], &xref_trace[0], 4))
				local_results.push_back({ start + i });
			break;

		default:
			break;
		}
	}

	if (!local_results.empty()) {
		return_vector_mutex.lock();
		for (const auto& c : local_results)
			return_vector.push_back(c);
		return_vector_mutex.unlock();
	}
}

void scanner_cfg_templates::function_xref_scan_internal_default(const scanner_args& args) {
	const auto& [proc, start, end, return_vector_mutex, return_vector, aob, mask, optargs] = args;
	const auto xref_trace = optargs.xref_trace_endianized;

	std::vector<scan_result> local_results;

	for (const auto* i = reinterpret_cast<std::uint8_t*>(start); i < reinterpret_cast<std::uint8_t*>(end); i++) {
		switch (*i) {
			// ABSOLUTE
			case 0x68: // PUSH Av
			case 0x9A: // CALL Az
			if (proc.is32 and !std::memcmp(i+1, &xref_trace[0], 4))
				local_results.push_back({ reinterpret_cast<std::uintptr_t>(i) });
			break;

			// RELATIVE
			case 0xE9: // JMP Jz
			case 0xE8: // CALL Jz
				if (reinterpret_cast<std::uintptr_t>(i) + 5 + *reinterpret_cast<const std::uint32_t*>(i+1) == optargs.xref_trace_int)
					local_results.push_back({ reinterpret_cast<std::uintptr_t>(i) });
			break;
			case 0x48: // 64bit operation
				if (!proc.is32)
					switch (*(i+1)) {
						case 0x8D: // LEA
							if (reinterpret_cast<std::uintptr_t>(i) + 7 + *reinterpret_cast<const std::uint32_t*>(i+1) == optargs.xref_trace_int)
								local_results.push_back({ reinterpret_cast<std::uintptr_t>(i) });

						break;
						default:break;
					}
			break;

			default:break;
		}
	}

	if (!local_results.empty()) {
		return_vector_mutex.lock();
		for (const auto& c : local_results)
			return_vector.push_back(c);
		return_vector_mutex.unlock();
	}
}

constexpr std::uint8_t prologue_sig_1[] = {0x89, 0x44, 0x24};
constexpr std::uint8_t prologue_sig_2[] = {0x55, 0x48, 0x83, 0xEC};
auto util::get_prologue(const process& proc, const std::uintptr_t func) -> std::expected<std::uintptr_t, std::string> {
	MEMORY_BASIC_INFORMATION mbi;

	if (proc.pid != GetCurrentProcessId()) {
		VirtualQueryEx(proc.curr_proc, reinterpret_cast<LPCVOID>(func), &mbi, sizeof MEMORY_BASIC_INFORMATION);

		const auto page_size = mbi.RegionSize;
		std::size_t n_read;
		std::vector<std::uint8_t> page_memory(page_size);
		const auto base_address = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
		ReadProcessMemory(proc.curr_proc, reinterpret_cast<LPCVOID>(base_address), page_memory.data(), page_size - (func - base_address), reinterpret_cast<SIZE_T*>(&n_read));

		for (auto loc = (func - (func % 16)) - base_address; loc > 0; loc -= 16) {
			switch (page_memory[loc]) {
			// 32-bit prologues
			case 0x53:
				if (proc.is32 && (page_memory[loc + 1] == 0x8B and ((page_memory[loc + 2] == 0xDC) or (page_memory[loc + 2] == 0xD9))) or (page_memory[loc + 1] == 0x56 and page_memory[loc + 2] == 0x8B and page_memory[loc + 3] == 0xD9))
					return base_address + loc;
				break;

			case 0x55:
				if (proc.is32 && page_memory[loc + 1] == 0x8B and page_memory[loc + 2] == 0xEC)
					return base_address + loc;
				break;

			case 0x56:
				if (proc.is32 && page_memory[loc + 1] == 0x8B and page_memory[loc + 2] == 0xF1)
					return base_address + loc;
				break;

			// 64-bit prologues
			case 0x4C:
				if (!proc.is32 && !std::memcmp(&page_memory[loc+1], &prologue_sig_1, 3))
					return base_address + loc;
				break;

			case 0x40:
				if (!proc.is32 && !std::memcmp(&page_memory[loc+1], &prologue_sig_2, 4))
					return base_address + loc;
				break;

			case 0x48:
				if (!proc.is32 && page_memory[loc+1] == 0x89 and (page_memory[loc+2] == 0x5C or page_memory[loc+2] == 0x4C or page_memory[loc+2] == 0x54) and page_memory[loc+3] == 0x24)
					return base_address + loc;
				break;
			default:
				break;
			}
		}

		// blanket fix for functions between two pages.
		return func - 512 > proc.proc_base+1000 ? get_prologue(proc, func - 512) : std::unexpected("No prologue found.");
	}

	VirtualQuery(reinterpret_cast<LPCVOID>(func), &mbi, sizeof MEMORY_BASIC_INFORMATION);

	for (const auto* loc = reinterpret_cast<std::uint8_t*>(func - func % 16); loc > mbi.BaseAddress; loc -= 16) {
		switch (*loc) {
			// 32-bit prologues
			case 0x53:
				if (proc.is32 && (*(loc + 1) == 0x8B and (*(loc + 2) == 0xDC or *(loc + 2) == 0xD9)) or (*(loc + 1) == 0x56 and *(loc + 2) == 0x8B and *(loc + 3) == 0xD9))
					return reinterpret_cast<std::uintptr_t>(loc);
			break;

			case 0x55:
				if (proc.is32 && *(loc + 1) == 0x8B and *(loc + 2) == 0xEC)
					return reinterpret_cast<std::uintptr_t>(loc);
			break;

			case 0x56:
				if (proc.is32 && *(loc + 1) == 0x8B and *(loc + 2) == 0xF1)
					return reinterpret_cast<std::uintptr_t>(loc);
			break;

			// 64-bit prologues
			case 0x4C:
				if (!proc.is32 && !std::memcmp(loc+1, &prologue_sig_1, 3))
					return reinterpret_cast<std::uintptr_t>(loc);
			break;

			case 0x40:
				if (!proc.is32 && !std::memcmp(loc+1, &prologue_sig_2, 4))
					return reinterpret_cast<std::uintptr_t>(loc);
			break;

			case 0x48:
				if (!proc.is32 && *(loc + 1) == 0x89 and (*(loc + 2) == 0x5C or *(loc + 2) == 0x4C or *(loc + 2) == 0x54) and *(loc + 3) == 0x24)
					return reinterpret_cast<std::uintptr_t>(loc);
			break;
			default:
				break;
		}
	}

	return std::unexpected("No prologue found.");
}

auto util::get_epilogue(const process& proc, const std::uintptr_t func, const bool all_alignment, const std::uint32_t min_alignment) -> std::expected<std::uintptr_t, std::string> {
	MEMORY_BASIC_INFORMATION mbi;
	if (proc.pid != GetCurrentProcessId()) {
		VirtualQueryEx(proc.curr_proc, reinterpret_cast<LPCVOID>(func), &mbi, sizeof MEMORY_BASIC_INFORMATION);

		const auto base_address = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
		const auto page_size = base_address + mbi.RegionSize - func;
		std::size_t n_read;
		std::vector<std::uint8_t> page_memory(page_size);
		auto remaining = 0ull;
		ReadProcessMemory(proc.curr_proc, reinterpret_cast<LPCVOID>(func), page_memory.data(), page_size, reinterpret_cast<SIZE_T*>(&n_read));

		for (auto loc = 0ull; loc < page_size; loc++)
		{
			switch (page_memory[loc])
			{
			case 0xC3:
				remaining = 15 - loc % 16;
				if (loc + remaining < page_size)
				{
					for (auto finding = loc+1; finding < (all_alignment ? loc+remaining+1 : loc+min_alignment+1); finding++)
					{
						if (page_memory[finding] == 0xCC or page_memory[finding] == 0x90)
							continue;

						goto out_of_bounds;
					}

					if (!remaining)
					{
						switch (page_memory[loc+1])
						{
						case 0x53:
							if (proc.is32 && !((page_memory[loc + 2] == 0x8B and ((page_memory[loc + 3] == 0xDC) or (page_memory[loc + 3] == 0xD9))) or (page_memory[loc + 2] == 0x56 and page_memory[loc + 3] == 0x8B and page_memory[loc + 4] == 0xD9)))
								goto out_of_bounds;
							break;

						case 0x55:
							if (proc.is32 && !(page_memory[loc + 1] == 0x8B and page_memory[loc + 2] == 0xEC))
								goto out_of_bounds;
						default:
							goto out_of_bounds;
							break;
						}
					}

					return func + loc + remaining;

					out_of_bounds:
					continue;
				default:
					break;
				}
				break;

			case 0xCC:
			case 0x90:
				remaining = 15 - ((loc - 1) % 16);
				if (loc + remaining < page_size) {
					for (auto finding = loc; finding < (all_alignment ? loc + remaining : loc + min_alignment); finding++) {
						if (page_memory[finding] != 0xCC and page_memory[finding] != 0x90)
							goto out_of_bounds_2;
					}

					if(remaining == 1 or !remaining) {
						switch (page_memory[loc + 1]) {
						case 0x53:
							if (proc.is32 && !((page_memory[loc + 2] == 0x8B and ((page_memory[loc + 3] == 0xDC) or (page_memory[loc + 3] == 0xD9))) or (page_memory[loc + 2] == 0x56 and page_memory[loc + 3] == 0x8B and page_memory[loc + 4] == 0xD9)))
								goto out_of_bounds_2;
							break;

						case 0x55:
							if (proc.is32 && !(page_memory[loc + 1] == 0x8B and page_memory[loc + 2] == 0xEC))
								goto out_of_bounds_2;
						default:
							goto out_of_bounds_2;
						}
					}

					return func + loc - 1;

				out_of_bounds_2:
					continue;
				}
			}
		}

		return std::unexpected("Could not find prologue.");
	}

	VirtualQuery(reinterpret_cast<LPCVOID>(func), &mbi, sizeof MEMORY_BASIC_INFORMATION);
	return std::unexpected("Not implemented.");
}

std::vector<scan_result> util::get_calls(const process& proc, const std::uintptr_t func) {
	MEMORY_BASIC_INFORMATION mbi;
	auto func_base = func;

	std::expected<std::uintptr_t, std::string> _fn_base_tmp;
	if (func_base % 16 != 0 && (_fn_base_tmp = get_prologue(proc, func_base)).has_value())
		func_base = *_fn_base_tmp;

	const auto _fn_end_tmp = get_epilogue(proc, func_base);
	if (!_fn_end_tmp.has_value())
		return {};

	const auto func_end = *_fn_end_tmp;

	std::vector<scan_result> scan_results;

	if (proc.pid != GetCurrentProcessId()) {
		VirtualQueryEx(proc.curr_proc, reinterpret_cast<LPCVOID>(func_base), &mbi, sizeof MEMORY_BASIC_INFORMATION);
		
		const auto func_sz = func_end - func_base + 1;

		std::size_t n_read;
		std::vector<std::uint8_t> page_memory(func_sz);
		ReadProcessMemory(proc.curr_proc, reinterpret_cast<LPCVOID>(func_base), page_memory.data(), func_sz, reinterpret_cast<SIZE_T*>(&n_read));

		std::uintptr_t rel_loc{0};
		for (auto loc = 0ull; loc < func_sz; loc++) {
			switch (page_memory[loc]) {
			case 0x9A: // CALL Az
					rel_loc = page_memory[loc + 1] | page_memory[loc + 2] << 8 | page_memory[loc + 3] << 16 | page_memory[loc + 4] << 24;
					if (rel_loc % 16 == 0)
						scan_results.push_back({ rel_loc });
				break;
			case 0xE8: // CALL Jz
					rel_loc = loc + func_base + 5 + (page_memory[loc + 1] | page_memory[loc + 2] << 8 | page_memory[loc + 3] << 16 | page_memory[loc + 4] << 24);
					if (rel_loc % 16 == 0)
						scan_results.push_back({ rel_loc });

			default:
				break;
			}
		}
	}
	else
	{
		VirtualQuery(reinterpret_cast<LPCVOID>(func), &mbi, sizeof MEMORY_BASIC_INFORMATION);
	}

	if (proc.is32)
		std::erase_if(scan_results, [](const scan_result result) { return result.loc > 0xFFFFFFFF;  });
	
	return scan_results;
}

std::vector<scan_result> util::get_jumps(const process& proc, const std::uintptr_t func, const bool functions_only, const bool include_twobyte_jmps)
{
	MEMORY_BASIC_INFORMATION mbi;
	auto func_base = func;
	std::expected<std::uintptr_t, std::string> _fn_base_tmp;
	if (func_base % 16 != 0 && (_fn_base_tmp = get_prologue(proc, func_base)).has_value())
		func_base = *_fn_base_tmp;

	const auto _fn_end_tmp = get_epilogue(proc, func_base);
	if (!_fn_end_tmp.has_value())
		return {};

	const auto func_end = *_fn_end_tmp;

	std::vector<scan_result> scan_results;


	if (proc.pid != GetCurrentProcessId()) {
		VirtualQueryEx(proc.curr_proc, reinterpret_cast<LPCVOID>(func_base), &mbi, sizeof MEMORY_BASIC_INFORMATION);

		const auto func_sz = func_end - func_base + 1;

		std::size_t n_read;
		std::vector<std::uint8_t> page_memory(func_sz);
		ReadProcessMemory(proc.curr_proc, reinterpret_cast<LPCVOID>(func_base), page_memory.data(), func_sz, reinterpret_cast<SIZE_T*>(&n_read));

		std::uintptr_t rel_loc{0};

		for (auto loc = 0ull; loc < func_sz; loc++) {
			switch (page_memory[loc]) {
			case 0xE9: // JMP Jz
				if (proc.is32) {
					rel_loc = loc + func_base + 5 + *reinterpret_cast<std::uint32_t*>(&page_memory[loc+1]);
					if (functions_only and (rel_loc % 16 != 0))
						break;

					scan_results.push_back({ rel_loc });
				}
				break;
			case 0xEA: // JMP Ap
				if (proc.is32) {
					rel_loc = *reinterpret_cast<std::uint32_t*>(&page_memory[loc+1]);
					if (functions_only and (rel_loc % 16 != 0))
						break;

					scan_results.push_back({ rel_loc });
				}
				break;

			/*case 0xEB: // JMP Jb
			case 0x70: // JO Jb
			case 0x71: // JNO Jb
			case 0x72: // JB Jb
			case 0x73: // JNB Jb
			case 0x74: // JZ Jb
			case 0x75: // JNZ Jb
			case 0x76: // JBE Jb
			case 0x77: // JA Jb
			case 0x78: // JS Jb
			case 0x79: // JNS Jb
			case 0x7A: // JP Jb
			case 0x7B: // JNP Jb
			case 0x7C: // JL Jb
			case 0x7D: // JNL Jb
			case 0x7E: // JLE Jb
			case 0x7F: // JNLE Jb
				if (include_twobyte_jmps)
				{
					rel_loc = func_base + loc + 2 + page_memory[loc + 1];
					if (functions_only and (rel_loc % 16 != 0))
						break;

					scan_results.push_back({ rel_loc });
				}
				break;
				*/

			default:
				break;
			}
		}
	}
	else {
		VirtualQuery(reinterpret_cast<LPCVOID>(func), &mbi, sizeof MEMORY_BASIC_INFORMATION);
	}

	return scan_results;
}
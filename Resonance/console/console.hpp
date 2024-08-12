#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>
#include <map>
#include <iomanip>
#include <ctime>
#include <chrono>


#define WHITE FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE

namespace color_schemes {
	namespace success {
		constexpr auto background = BACKGROUND_GREEN | BACKGROUND_INTENSITY;
		constexpr auto foreground = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	}

	namespace info {
		constexpr auto background = BACKGROUND_INTENSITY;
		constexpr auto foreground = FOREGROUND_INTENSITY;
	}

	namespace warn {
		constexpr auto background = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN;
		constexpr auto foreground = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
	}
	namespace error {
		constexpr auto background = BACKGROUND_RED | BACKGROUND_INTENSITY;
		constexpr auto foreground = FOREGROUND_RED | FOREGROUND_INTENSITY;
	}
}


enum class log_severity {
	info,
	warn,
	error,
	success,
};

static inline std::map<log_severity, std::tuple<int, int, std::string_view>> color_schemes_map {
	{
		log_severity::info,
		{ color_schemes::info::background, color_schemes::info::foreground, "INFO" }
	}, {
		log_severity::warn,
		{ color_schemes::warn::background, color_schemes::warn::foreground, "WARN" }
	}, {
		log_severity::error,
		{ color_schemes::error::background, color_schemes::error::foreground, "ERROR" }
	}, {
		log_severity::success,
		{ color_schemes::success::background, color_schemes::success::foreground, "SUCCESS" }
	}
};

struct console {

	console() noexcept {
		AllocConsole();
		FILE* con_shi;
		freopen_s(&con_shi, "CONOUT$", "w", stdout);
		SetWindowPos(GetConsoleWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		ShowWindow(GetConsoleWindow(), SW_NORMAL);
		SetConsoleTitleA("Resonance | 1.0.0");
	}
	
	~console() {
		FreeConsole();
	}

	template <log_severity Sev, typename... Va>
	static void log(const std::string_view fmt, Va&&... vargs)
	{
		static auto con_wnd = GetStdHandle(STD_OUTPUT_HANDLE);
		const auto& [bg, fg, tag] = color_schemes_map[Sev];
		SetConsoleTextAttribute(con_wnd, bg);
		std::cout << "  ";
		SetConsoleTextAttribute(con_wnd, fg);
		const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		struct tm buf;
		localtime_s(&buf, &now);
		std::printf((std::string(" [%02d:%02d:%02d] [%s]: ").append(fmt) + "\n").c_str(), buf.tm_hour, buf.tm_min, buf.tm_sec, tag.data(), std::forward<Va>(vargs)...);
	}
};
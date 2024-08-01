#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>
#include <iomanip>
#include <ctime>
#include <chrono>


#define WHITE FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE

enum class log_severity {
	info,
	warn,
	error,
	success,
};

struct console {

	console() noexcept {
		AllocConsole();
		FILE* con_shi;
		freopen_s(&con_shi, "CONOUT$", "w", stdout);
		freopen_s(&con_shi, "CONIN$", "r", stdin);
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
		const auto [bg, fg, tag] = resolve_severity(Sev);
		SetConsoleTextAttribute(con_wnd, bg);
		std::cout << "  ";
		SetConsoleTextAttribute(con_wnd, fg);
		const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		struct tm buf;
		localtime_s(&buf, &now);
		std::printf((std::string(" [%02d:%02d:%02d] [%s]: ").append(fmt) + "\n").c_str(), buf.tm_hour, buf.tm_min, buf.tm_sec, tag, std::forward<Va>(vargs)...);
	}

private:
	static constexpr std::tuple<int, int, std::string > resolve_severity(const log_severity sev)
	{
		switch (sev)
		{
		case log_severity::info:
			return { BACKGROUND_INTENSITY, FOREGROUND_INTENSITY, "INFO" };
		case log_severity::warn:
			return { BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN, "WARN" };
		case log_severity::success:
			return { BACKGROUND_GREEN | BACKGROUND_INTENSITY, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "SUCCESS" };
		case log_severity::error:
			return { BACKGROUND_RED | BACKGROUND_INTENSITY, FOREGROUND_RED | FOREGROUND_INTENSITY, "ERROR" };
		}
		return { 0, WHITE, "general" };
	}
};
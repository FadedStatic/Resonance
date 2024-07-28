
#include "include.hpp"

void console::init()
{
	AllocConsole();
	FILE* con_shi;
	freopen_s(&con_shi, "CONOUT$", "w", stdout);
	freopen_s(&con_shi, "CONIN$", "r", stdin);
	SetWindowPos(GetConsoleWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	ShowWindow(GetConsoleWindow(), SW_NORMAL);
}
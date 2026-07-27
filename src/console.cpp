#include "console.h"
#include <iostream>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#endif

void console_init() {
#ifdef _WIN32
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &mode)) {
		SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	}
#endif
}

void clear_screen() {
	std::cout << "\x1b[2J\x1b[H" << std::flush;
}

void pause_console() {
	std::cout << "Press Enter to continue...";
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

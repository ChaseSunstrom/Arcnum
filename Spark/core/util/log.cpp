#include "log.hpp"

#ifdef _WIN32
#	include <windows.h>

void set_console_color(u32 color) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); }

#else

void set_console_color(uint32_t color) {}

#endif
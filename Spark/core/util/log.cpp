#include "log.hpp"

#include <core/util/time.hpp>

#ifdef _WIN32
#	include <windows.h>

void SetConsoleColor(u32 color) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); }

#else

void SetConsoleColor(u32 color) {}

#endif

#ifdef _WIN32
// Annoying macro windows uses
#	undef GetCurrentTime
#endif

void LogTime() { std::cout << _SPARK GetCurrentTime() << " "; }
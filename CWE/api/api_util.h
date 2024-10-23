#pragma once
#include <cstdarg>
#include <stdlib.h>
#include <stdio.h>

struct APIErrorUtil {
	char prefix[255];

	APIErrorUtil(const char* format, ...) {
		va_list argptr;
		va_start(argptr, format);
		vsnprintf(prefix, _countof(prefix), format, argptr);
		va_end(argptr);
	}

	void print(const char* format, ...) {
		char formatBuff[255], buff[255];

		va_list argptr;
		va_start(argptr, format);
		vsnprintf(formatBuff, _countof(formatBuff), format, argptr);
		va_end(argptr);

		sprintf_s(buff, "%s%s", prefix, formatBuff);

		MessageBoxA(0, buff, "CWE", 0);
	}
};

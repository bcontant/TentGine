#include <stdarg.h>
#include "StringUtils.h"

std::string Format(const char* format, ...)
{
	char strMessage[1024];

	va_list ArgPtr;

	va_start(ArgPtr, format);
	vsnprintf(strMessage, 1024, format, ArgPtr);
	va_end(ArgPtr);

	return strMessage;
}

std::wstring Format(const wchar_t* format, ...)
{
	wchar_t strMessage[1024];

	va_list ArgPtr;

	va_start(ArgPtr, format);
	vswprintf(strMessage, 1024, format, ArgPtr);
	va_end(ArgPtr);

	return strMessage;
}
#include "precompiled.h"

//--------------------------------------------------------------------------------
std::string WStringToString(std::wstring in_wstring)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(in_wstring);
}

//--------------------------------------------------------------------------------
std::wstring StringToWString(std::string in_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(in_string);
}

//--------------------------------------------------------------------------------
StdString Format(const StringChar* format, ...)
{
	StringChar strMessage[1024];

	va_list ArgPtr;

	va_start(ArgPtr, format);
	VSPRINTF(strMessage, 1024, format, ArgPtr);
	va_end(ArgPtr);

	return strMessage;
}
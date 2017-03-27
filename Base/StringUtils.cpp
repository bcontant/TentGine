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
std_string Format(const string_char* format, ...)
{
	string_char strMessage[1024];

	va_list ArgPtr;

	va_start(ArgPtr, format);
	VSPRINTF(strMessage, 1024, format, ArgPtr);
	va_end(ArgPtr);

	return strMessage;
}

namespace StringUtils
{

	//--------------------------------------------------------------------------------
	s32 atoi32(const string_char* in_string)
	{
		s32 value;
		SSCANF(in_string, L("%d"), &value);
		return value;
	}

	//--------------------------------------------------------------------------------
	s64 atoi64(const string_char* in_string)
	{
		s64 value;
		SSCANF(in_string, L("%llx"), &value);
		return value;
	}

	//--------------------------------------------------------------------------------
	float atof(const string_char* in_string)
	{
		float value;
		SSCANF(in_string, L("%f"), &value);
		return value;
	}

}
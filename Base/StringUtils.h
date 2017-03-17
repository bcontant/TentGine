#pragma once

#include <string>
#include <stdarg.h>
#include <stdio.h>

#ifdef _UNICODE
#define WIDE(x) L##x
#define L(x) WIDE(x)

#define VSPRINTF vswprintf
#define VFPRINTF vfwprintf_s

#define STRLEN wcslen
#define STRCPY wcscpy_s
#define STRCAT wcscat_s

#define FOPEN _wfopen_s

#define TO_WSTRING std_string
#define TO_STRING WStringToString
#define FROM_STRING StringToWString
#define FROM_WSTRING std_string

#else //#ifdef _UNICODE
#define L(x) x

#define VSPRINTF vsnprintf
#define VFPRINTF vfprintf_s
#define STRLEN strlen
#define STRCPY strcpy_s
#define STRCAT strcat_s

#define FOPEN fopen_s

#define TO_WSTRING StringToWString
#define TO_STRING std_string
#define FROM_STRING std_string
#define FROM_WSTRING WStringToString

#endif //#ifdef _UNICODE

std::string WStringToString(std::wstring in_wstring);
std::wstring StringToWString(std::string in_string);

std_string Format(const string_char* format, ...);

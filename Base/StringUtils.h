#pragma once

#include <string>
#include <stdarg.h>
#include <stdio.h>

#ifdef _UNICODE
#define WIDE(x) L##x
#define L(x) WIDE(x)

typedef std::wstring StdString;
typedef wchar_t StringChar;
typedef wchar_t UStringChar;

#define VSPRINTF vswprintf
#define VFPRINTF vfwprintf_s

#define STRLEN wcslen
#define STRCPY wcscpy_s
#define STRCAT wcscat_s

#define FOPEN _wfopen_s

#define TO_WSTRING StdString
#define TO_STRING WStringToString
#define FROM_STRING StringToWString
#define FROM_WSTRING StdString

#else
#define L(x) x

typedef std::string StdString;
typedef char StringChar;
typedef unsigned char UStringChar;

#define VSPRINTF vsnprintf
#define VFPRINTF vfprintf_s
#define STRLEN strlen
#define STRCPY strcpy_s
#define STRCAT strcat_s

#define FOPEN fopen_s

#define TO_WSTRING StringToWString
#define TO_STRING StdString
#define FROM_STRING StdString
#define FROM_WSTRING WStringToString

#endif

std::string WStringToString(std::wstring in_wstring);
std::wstring StringToWString(std::string in_string);

StdString Format(const StringChar* format, ...);

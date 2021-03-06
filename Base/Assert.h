#pragma once

#include "StringUtils.h"
#include "../OS_Base/System.h"

//Custom assert function, used for extra flexibility in when and how to break, and the added
//message as to why the assert failed, and where.
//The XYZ is used to make sure the internal define variables don't clash with variables in the source code

//--------------------------------------------------------------------------------
#ifdef _MSC_VER
	#define DEBUGGER_BREAK __debugbreak()
#else 
	#define DEBUGGER_BREAK asm("int $3")
#endif

#ifndef _RETAIL

//--------------------------------------------------------------------------------
#define ASSERT_MESSAGE_BUFFER_SIZE  (16384)

//--------------------------------------------------------------------------------
extern bool g_bInsideAssertXYZ;

//--------------------------------------------------------------------------------
s32 CustomAssertFunction(const string_char* in_expstr, const string_char* in_file, const string_char* in_function, const s32 in_line, const string_char* in_desc = L(""), ...);

//--------------------------------------------------------------------------------
#define AssertMsg(condition, format, ...) \
do { \
    static bool bAssertAlwaysIgnore = false; \
    if( !bAssertAlwaysIgnore && !(condition) ) \
    { \
		g_bInsideAssertXYZ = true; \
        s32 iReturnValueAssert = CustomAssertFunction(L(#condition), L(__FILE__), L(__FUNCTION__), __LINE__, format, ##__VA_ARGS__); \
        if( iReturnValueAssert == 1)  \
        { \
			DEBUGGER_BREAK; \
        } \
        if( iReturnValueAssert == 2 ) \
        { \
            bAssertAlwaysIgnore = true; \
        } \
		g_bInsideAssertXYZ = false; \
    } \
} while(false)

//--------------------------------------------------------------------------------
#define Assert(condition) AssertMsg(condition, nullptr)

#else

//--------------------------------------------------------------------------------
#define AssertMsg(condition, format, ...) { ( false ? (void)(condition) : (void)0 ); }
#define Assert(condition) AssertMsg(condition, nullptr)

#endif  //#ifndef _RETAIL*/

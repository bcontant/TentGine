#pragma once

#include "StringUtils.h"
#include "../OS_Base/System.h"

//Custom assert function, used for extra flexibility in when and how to break, and the added
//message as to why the assert failed, and where.
//The XYZ is used to make sure the internal define variables don't clash with variables in the source code

#ifdef _MSC_VER
	#define DEBUGGER_BREAK __debugbreak()
#else
	#define DEBUGGER_BREAK asm("int $3")
#endif //_MSC_VER*/

#ifdef _DEBUG

#define ASSERT_MESSAGE_BUFFER_SIZE  (16384)
#define ASSERT_FILENAME_SIZE        (1024)

extern StdString g_strAssertFileXYZ;
extern int g_iAssertLineXYZ;
extern bool g_bInsideAssertXYZ;

int CustomAssertFunction(const StringChar* in_expstr, const StringChar* in_desc = L(""), ...);

#define AssertMsg(condition, format, ...) \
{ \
    static bool bAssertAlwaysIgnore = false; \
    if( !bAssertAlwaysIgnore && !(condition) ) \
    { \
        g_strAssertFileXYZ = L(__FILE__); \
        g_iAssertLineXYZ = __LINE__; \
        int iReturnValueAssert = CustomAssertFunction(L(#condition), format, ##__VA_ARGS__); \
        if( iReturnValueAssert == 1)  \
        { \
			DEBUGGER_BREAK; \
        } \
        if( iReturnValueAssert == 2 ) \
        { \
            bAssertAlwaysIgnore = true; \
        } \
    } \
}


#else

#define AssertMsg(condition, format, ...) {}

#endif  //#ifdef _DEBUG*/

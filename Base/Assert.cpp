#include "precompiled.h"

#include "../OS_Base/System.h"

#ifdef _DEBUG

//--------------------------------------------------------------------------------
//Prevent re-entrance from the Logger
bool g_bInsideAssertXYZ = false;

//------------------------------------------------------------------------------
int CustomAssertFunction(const StringChar* in_expstr, const StringChar* in_file, const StringChar* in_function, const int in_line, const StringChar* in_desc, ...)
{
	//Temp strings to store the MessageBox error
	static StringChar strFormattedMessage[ASSERT_MESSAGE_BUFFER_SIZE];
	StdString strMessage;
	StdString strLocation;

	//If the assertion is false and we're not ignoring
	va_list ArgPtr;

	if (in_desc != nullptr)
	{
		va_start(ArgPtr, in_desc);
		VSPRINTF(strFormattedMessage, ASSERT_MESSAGE_BUFFER_SIZE, in_desc, ArgPtr);
		va_end(ArgPtr);
	}

	std::vector<StdString> stackTrace = OS::GetCallStack();
	
	if (Logger::GetInstance())
	{
		strMessage = L("\n***************\nASSERTION FAILED : \n");
		strMessage += Format(L("[%s]\n"), in_expstr);
		if(in_desc != nullptr)
			strMessage += Format(L("%s : %s\n"), in_function, strFormattedMessage);
		strMessage += Format(L("%s(%d)\n"), in_file, in_line);
		strMessage += L("Stack Trace:\n");
		for (unsigned int i = 1; i < stackTrace.size(); i++)
			strMessage += stackTrace[i] + L("\n");
		strMessage += L("***************");

		Logger::GetInstance()->Log(eLC_Assert, eLS_Error, eLT_FileAndDebug, strMessage.c_str());
	}

	strMessage = Format(L("[%s]\n\n"), in_expstr);
	if (in_desc != nullptr)
		strMessage += Format(L("%s : %s\n\n"), in_function, strFormattedMessage);
	strMessage += Format(L("%s(%d)\n\n"), in_file, in_line);
	strMessage += L("Stack Trace:\n");
	for (unsigned int i = 1; i < stackTrace.size(); i++)
		strMessage += stackTrace[i] + L("\n");

	int iRet = OS::ShowMessageBox(L("ASSERTION FAILED"), strMessage.c_str(), OS::eAbort | OS::eRetry | OS::eIgnore);
	switch (iRet)
	{
	case(OS::eAbort) :
		exit(1);
		return 1;
	case(OS::eRetry) :
		return 1;
	case(OS::eIgnore) :
		if (OS::LShiftDown())
			return 2;
		return 0;
	default:
		return 1;
	}
}

#endif



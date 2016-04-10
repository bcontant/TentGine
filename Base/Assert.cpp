#include "precompiled.h"

#include "../OS_Base/System.h"

#ifdef _DEBUG

//--------------------------------------------------------------------------------
StdString g_strAssertFileXYZ;
int g_iAssertLineXYZ = 0;
bool g_bInsideAssertXYZ = false;

//------------------------------------------------------------------------------
int CustomAssertFunction(const StringChar* in_expstr, const StringChar* in_desc, ...)
{
	//Temp strings to store the MessageBox error
	static StringChar strFormattedMessage[ASSERT_MESSAGE_BUFFER_SIZE];
	StdString strMessage;
	StdString strLocation;

	g_bInsideAssertXYZ = true;

	//If the assertion is false and we're not ignoring
	va_list ArgPtr;

	va_start(ArgPtr, in_desc);
	VSPRINTF(strFormattedMessage, ASSERT_MESSAGE_BUFFER_SIZE, in_desc, ArgPtr);
	va_end(ArgPtr);

	strLocation = Format(L("%s, line %d"), g_strAssertFileXYZ.c_str(), g_iAssertLineXYZ);

	strMessage = L("\n***************\nASSERTION FAILED : \n");
	strMessage += Format(L("[%s]\n"), in_expstr);
	strMessage += Format(L("%s\n"), strFormattedMessage);
	strMessage += Format(L("(%s)\n"), strLocation.c_str());

	/*strcat_s(strMessage, ASSERT_MESSAGE_BUFFER_SIZE, "Stack Trace:\n");

	if (StackTrace::GetInstance())
	{
		stackTrace = StackTrace::GetInstance()->GetStrackTrace();
		for (unsigned int i = 0; i < stackTrace.size(); i++)
		{
			strcat_s(strMessage, ASSERT_MESSAGE_BUFFER_SIZE, stackTrace[i].c_str());
			strcat_s(strMessage, ASSERT_MESSAGE_BUFFER_SIZE, "\n");
		}
	}*/

	strMessage += L("***************");

	if (Logger::GetInstance())
	{
		Logger::GetInstance()->Log(eLC_Assert, eLS_Error, eLT_FileAndDebug, strMessage.c_str());
	}

	g_bInsideAssertXYZ = false;
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



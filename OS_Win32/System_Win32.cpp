#include "precompiled.h"

#pragma warning(push)
#pragma warning(disable: 4091)
#include "DbgHelp.h"
#pragma warning(pop)

namespace OS
{
	void DebugOut(StringChar* in_pMessage)
	{
		OutputDebugString(in_pMessage);
	}

	int ShowMessageBox(const StringChar* title, const StringChar* message, unsigned int buttons)
	{
		UINT type = 0;
		if (buttons == (eAbort | eRetry | eIgnore) )
			type = MB_ABORTRETRYIGNORE;
		else if (buttons == (eOk) )
			type = MB_OK;
		else if (buttons == (eOk | eCancel) )
			type = MB_OKCANCEL;
		else if (buttons == (eYes | eNo) )
			type = MB_YESNO;
		else if (buttons == (eYes | eNo | eCancel) )
			type = MB_YESNOCANCEL;
		else
			return -1;
		
		int iRet = MessageBox(NULL, message, title, type);
		switch (iRet)
		{
		case (IDOK) :
			return eOk;
		case (IDCANCEL) :
			return eCancel;
		case(IDABORT) :
			return eAbort;
		case(IDRETRY) :
			return eRetry;
		case(IDIGNORE) :
			return eIgnore;
		case (IDYES) :
			return eYes;
		case (IDNO) :
			return eNo;
		}
		return -1;
	}

	bool LShiftDown()
	{
		return (GetAsyncKeyState(VK_LSHIFT) & 0x8000) > 0;
	}

	std::vector<StdString> GetCallStack(void* sample_address)
	{
		std::vector<StdString> stackTrace;
		
		HANDLE process = GetCurrentProcess();
		SymInitialize(process, NULL, TRUE);

		// ## The sum of the FramesToSkip and FramesToCapture parameters must be less than 63.
		const int kMaxCallers = 62;
		void* callers_stack[kMaxCallers];
		unsigned short frames = RtlCaptureStackBackTrace(0, kMaxCallers, callers_stack, NULL);

		SYMBOL_INFO* symbol = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
		symbol->MaxNameLen = 255;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

		for (unsigned int i = 1; i < frames; i++)
		{
			SymFromAddr(process, (DWORD64)(callers_stack[i]), 0, symbol);
			StdString function = FROM_STRING(symbol->Name);
			stackTrace.push_back(Format(L(" %s (0x%0X)"), function.c_str(), symbol->Address));

			if (function == L("main") || function == L("WinMain"))
				break;
		}

		free(symbol);
		return stackTrace;
	}

	__int64 GetTickCount()
	{
		__int64 t;
		QueryPerformanceCounter((LARGE_INTEGER*)&t);
		return t;
	}

	__int64 GetTickFrequency()
	{
		static __int64 freq = 0;

		if(freq == 0)
			QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

		return freq;
	}


}


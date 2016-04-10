#include "precompiled.h"

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
}
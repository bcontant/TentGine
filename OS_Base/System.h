#pragma once

#include "../Base/StringUtils.h"

namespace OS
{
	void DebugOut(StringChar* in_pMessage);

	enum PopupButtons
	{
		eOk = 1 << 0,
		eCancel = 1 << 1,
		eAbort = 1 << 2,
		eRetry = 1 << 3,
		eIgnore = 1 << 4,
		eYes = 1 << 5,
		eNo = 1 << 6,
	};

	int ShowMessageBox(const StringChar* title, const StringChar* message, unsigned int buttons);

	bool LShiftDown();
}
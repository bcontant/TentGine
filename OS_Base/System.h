#pragma once

#include <vector>
#include "../Base/StringUtils.h"
#include "../Base/Path.h"

namespace OS
{
	//Debugging
	void DebugOut(string_char* in_pMessage);
	std::vector<std_string> GetCallStack();

	//Dialogs
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
	s32 ShowMessageBox(const string_char* title, const string_char* message, u32 buttons);

	//Input
	bool LShiftDown();
	
	//Timer
	s64 GetTickCount();
	s64 GetTickFrequency();

	//Threading

	//FileSystem
	std::vector<Path> GetFileList(const Path& folderPath, const std_string fileExtension, bool in_bRecurseSubFolders);
}
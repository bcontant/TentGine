#include "precompiled.h"

#pragma warning(push)
#pragma warning(disable: 4091)
#include "DbgHelp.h"
#pragma warning(pop)

namespace OS
{
	//--------------------------------------------------------------------------------
	void DebugOut(string_char* in_pMessage)
	{
		OutputDebugString(in_pMessage);
	}

	//--------------------------------------------------------------------------------
	s32 ShowMessageBox(const string_char* title, const string_char* message, u32 buttons)
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
		
		s32 iRet = MessageBox(nullptr, message, title, type);
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

	//--------------------------------------------------------------------------------
	bool LShiftDown()
	{
		return (GetAsyncKeyState(VK_LSHIFT) & 0x8000) > 0;
	}

	//--------------------------------------------------------------------------------
	std::vector<std_string> GetCallStack()
	{
		std::vector<std_string> stackTrace;
		
		HANDLE process = GetCurrentProcess();
		SymInitialize(process, nullptr, TRUE);

		// ## The sum of the FramesToSkip and FramesToCapture parameters must be less than 63.
		const s32 kMaxCallers = 62;
		void* callers_stack[kMaxCallers];
		u16 frames = RtlCaptureStackBackTrace(0, kMaxCallers, callers_stack, nullptr);

		SYMBOL_INFO* symbol = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(s8), 1);
		symbol->MaxNameLen = 255;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

		for (u32 i = 1; i < frames; i++)
		{
			SymFromAddr(process, (DWORD64)(callers_stack[i]), 0, symbol);
			std_string function = FROM_STRING(symbol->Name);
			stackTrace.push_back(Format(L(" %s (0x%0X)"), function.c_str(), symbol->Address));

			if (function == L("main") || function == L("WinMain"))
				break;
		}

		free(symbol);
		return stackTrace;
	}

	//--------------------------------------------------------------------------------
	s64 GetTickCount()
	{
		s64 t;
		QueryPerformanceCounter((LARGE_INTEGER*)&t);
		return t;
	}

	//--------------------------------------------------------------------------------
	s64 GetTickFrequency()
	{
		static s64 freq = 0;

		if(freq == 0)
			QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

		return freq;
	}

	//--------------------------------------------------------------------------------
	std::vector<Path> GetFileList(const Path& folderPath, const std_string fileExtension, bool in_bRecurseSubFolders)
	{
		// Find out if we're looking for specific search pattern
		Path searchPattern;
		if (fileExtension[0] == '.')
			searchPattern = folderPath + L("/*") + fileExtension;
		else
			searchPattern = folderPath + L("/*.") + fileExtension;

		// Create the vector to fill
		std::vector<Path> filenames;

		// Now use those windows functions to list the files
		WIN32_FIND_DATA findInfo;
		HANDLE findHandle = FindFirstFile(searchPattern.GetData(), &findInfo);
		BOOL keepSearching = (findHandle != INVALID_HANDLE_VALUE);
		while (keepSearching)
		{
			if ((findInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				filenames.push_back(findInfo.cFileName);
			}

			// Pick up the next file
			keepSearching = FindNextFile(findHandle, &findInfo);
		}

		// Clean up and leave
		FindClose(findHandle);

		//Recurse sub-folders
		if (in_bRecurseSubFolders)
		{
			searchPattern = folderPath + L("/*.");
			findHandle = FindFirstFile(searchPattern.GetData(), &findInfo);
			keepSearching = (findHandle != INVALID_HANDLE_VALUE);
			while (keepSearching)
			{
				if ((findInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
				{
					if (STRCMP(findInfo.cFileName, L(".")) != 0 && STRCMP(findInfo.cFileName, L("..")) != 0)
					{
						std::vector<Path> subfilenames = OS::GetFileList(folderPath + findInfo.cFileName + L("/"), fileExtension, in_bRecurseSubFolders);
						for (unsigned int i = 0; i < subfilenames.size(); i++)
						{
							Path path = findInfo.cFileName;
							path += L("/");
							filenames.push_back(path + subfilenames[i]);
						}
					}
				}

				// Pick up the next file
				keepSearching = FindNextFile(findHandle, &findInfo);
			}

			FindClose(findHandle);
		}


		return filenames;
	}
}


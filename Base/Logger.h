#pragma once

#include "Singleton.h"
#include "File.h"

enum ELogCategory
{
	eLC_Init,
	eLC_Shutdown,

	eLC_System,
	eLC_FileSystem,

	eLC_Profiler,
	eLC_Assert,
	eLC_DebugInfo,

	eLC_Input,
	eLC_Rendering,

	eLogCategory_Count
};
extern const StringChar* kLogCategories[];

enum ELogSeverity
{
	eLS_Message,
	eLS_Warning,
	eLS_Error,
	eLogSeverity_Count
};
extern const StringChar* kLogSeverities[];

enum ELogType
{
	eLT_File,
	eLT_FileAndDebug,
	eLogType_Count
};

class Logger : public Singleton<Logger>
{
	MAKE_SINGLETON(Logger);

public:
	void Log(ELogCategory in_eCategory, ELogSeverity in_eSeverity, ELogType in_eLogType, const StringChar* in_pMsg, ...);

protected:
	virtual void Initialize();

private:
	Logger();
	virtual ~Logger();

	void LogFormattedMsg(ELogCategory in_eCategory, ELogSeverity in_eSeverity, ELogType in_eLogType, StringChar* in_pMsg);

	File m_LogFile;
};

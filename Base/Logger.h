#pragma once

#include "Singleton.h"
#include "File.h"

//--------------------------------------------------------------------------------
enum class LogCategory
{
	Init,
	Shutdown,
	System,
	FileSystem,
	Assert,
	DebugInfo,
	Input,
	Rendering
};

//--------------------------------------------------------------------------------
enum class LogSeverity
{
	Message,
	Warning,
	Error
};

//--------------------------------------------------------------------------------
enum class LogType
{
	File,
	FileAndDebug
};

//--------------------------------------------------------------------------------
class Logger : public Singleton<Logger>
{
	MAKE_SINGLETON(Logger);

public:
	void Log(LogCategory in_eCategory, LogSeverity in_eSeverity, LogType in_eLogType, const StringChar* in_pMsg, ...);

protected:
	virtual void Initialize();

private:
	Logger();
	virtual ~Logger();

	void LogFormattedMsg(LogCategory in_eCategory, LogSeverity in_eSeverity, LogType in_eLogType, StringChar* in_pMsg);

	File m_LogFile;
};

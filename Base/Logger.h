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
	void Log(LogCategory in_eCategory, LogSeverity in_eSeverity, LogType in_eLogType, const string_char* in_pMsg, ...);

protected:
	virtual void Initialize();

private:
	Logger();
	virtual ~Logger();

	void LogFormattedMsg(LogCategory in_eCategory, LogSeverity in_eSeverity, LogType in_eLogType, string_char* in_pMsg);

	File m_LogFile;
};

#define DEBUG_LOG(format, ...) \
	Logger::GetInstance()->Log(LogCategory::DebugInfo, LogSeverity::Message, LogType::FileAndDebug, format, ##__VA_ARGS__);

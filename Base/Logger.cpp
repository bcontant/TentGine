#include "precompiled.h"

#include "../OS_Base/System.h"

//--------------------------------------------------------------------------------
static const std::map<LogCategory, std::string> kLogCategories
{
	std::make_pair(LogCategory::Init,		("Init        ")),
	std::make_pair(LogCategory::Shutdown,	("Shutdown    ")),
	std::make_pair(LogCategory::System,		("System      ")),
	std::make_pair(LogCategory::FileSystem,	("FileSystem  ")),
	std::make_pair(LogCategory::Assert,		("Assert      ")),
	std::make_pair(LogCategory::DebugInfo,	("Debug Info  ")),
	std::make_pair(LogCategory::Input,		("Input       ")),
	std::make_pair(LogCategory::Rendering,	("Rendering   ")),
};

//--------------------------------------------------------------------------------
static const std::map<LogSeverity, std::string> kLogSeverities =
{
	std::make_pair(LogSeverity::Message,	("Message  ")),
	std::make_pair(LogSeverity::Warning,	("Warning  ")),
	std::make_pair(LogSeverity::Error,		("Error    ")),
};

//--------------------------------------------------------------------------------
Logger::Logger()
{
}

//--------------------------------------------------------------------------------
Logger::~Logger()
{
	m_LogFile.Close();
}

//--------------------------------------------------------------------------------
void Logger::Initialize()
{
	m_LogFile.Open(L("log.txt"), FileMode::WriteText);
	Logger::GetInstance()->Log(LogCategory::Init, LogSeverity::Message, LogType::FileAndDebug, L("LogSystem Initialized"));
}

//--------------------------------------------------------------------------------
void Logger::Log(LogCategory in_eCategory, LogSeverity in_eSeverity, LogType in_eLogType, const string_char* in_pMsg, ...)
{
	if (this == nullptr)
		return;

	static string_char strTemp[16384];

	va_list ArgPtr;
	va_start(ArgPtr, in_pMsg);
	VSPRINTF(strTemp, 16384, in_pMsg, ArgPtr);
	va_end(ArgPtr);

	LogFormattedMsg(in_eCategory, in_eSeverity, in_eLogType, strTemp);
}

//--------------------------------------------------------------------------------
void Logger::LogFormattedMsg(LogCategory in_eCategory, LogSeverity in_eSeverity, LogType in_eLogType, string_char* in_pMsg)
{
	//Do the "Debug" part of the logType
	if (in_eLogType == LogType::FileAndDebug)
	{
		OS::DebugOut(in_pMsg);
		OS::DebugOut(L("\n"));
	}

	STRCAT(in_pMsg, 16384, L("\n"));
	string_char* str = in_pMsg;
	std::vector<std_string> logMessages;

	//Clean up linefeeds for the logfile, all log messages should be single line.
	while (*str != '\0')
	{
		s32 i = 0;
		while (str[i] != '\n' && str[i] != '\0')
			i++;

		if (str[i] == '\n')
		{
			str[i] = '\0';
			i++;
		}

		if (STRLEN(str) > 0)
			logMessages.push_back(str);

		str = &str[i];
	}

	//Do the "File" part of the logType
	for (u32 i = 0; i < logMessages.size(); i++)
	{
		//if (TimeManager::GetInstance())
		//	m_CurrentLogSize += m_pLogFile->Print("[%15.6f\t] ", TimeManager::GetInstance()->GetCurrentTimeInSeconds());

		m_LogFile.Print(L("(%s) "), kLogCategories.at(in_eCategory).c_str());
		m_LogFile.Print(L("(%s) "), kLogSeverities.at(in_eSeverity).c_str());
		m_LogFile.Print(logMessages[i].c_str());
		m_LogFile.Print(L("\n"));
	}

	m_LogFile.Flush();
}


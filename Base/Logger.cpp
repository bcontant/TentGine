#include "precompiled.h"

#include "../OS_Base/System.h"

const StringChar* kLogCategories[] =
{
	L("Init        "),
	L("Shutdown    "),
	L("System      "),
	L("FileSystem  "),
	L("Profiler    "),
	L("Assert      "),
	L("Debug Info  "),
	L("Input       "),
	L("Rendering   "),
};

const StringChar* kLogSeverities[] =
{
	L("Message  "),
	L("Warning  "),
	L("Error    ")
};


Logger::Logger()
{
}

Logger::~Logger()
{
	m_LogFile.Close();
}

void Logger::Initialize()
{
	m_LogFile.Open(L("log.txt"), File::fmWriteText);
	Logger::GetInstance()->Log(eLC_Init, eLS_Message, eLT_FileAndDebug, L("LogSystem Initialized"));
	//Logger::GetInstance()->Log("LogFile created on %02d-%02d-%04d at %02d:%02d:%02d -- Part %d", sysTime.GetMonth(), sysTime.GetDay(), sysTime.GetYear(), sysTime.GetHour(), sysTime.GetMinute(), sysTime.GetSecond(), m_LogFileNumber);
}

void Logger::Log(ELogCategory in_eCategory, ELogSeverity in_eSeverity, ELogType in_eLogType, const StringChar* in_pMsg, ...)
{
	if (this == nullptr)
		return;

	static StringChar strTemp[16384];

	va_list ArgPtr;
	va_start(ArgPtr, in_pMsg);
	VSPRINTF(strTemp, 16384, in_pMsg, ArgPtr);
	va_end(ArgPtr);

	LogFormattedMsg(in_eCategory, in_eSeverity, in_eLogType, strTemp);
}

void Logger::LogFormattedMsg(ELogCategory in_eCategory, ELogSeverity in_eSeverity, ELogType in_eLogType, StringChar* in_pMsg)
{
	//Do the "Debug" part of the logType
	if (in_eLogType == eLT_FileAndDebug)
	{
		OS::DebugOut(in_pMsg);
		OS::DebugOut(L("\n"));
	}

	STRCAT(in_pMsg, 16384, L("\n"));
	StringChar* str = in_pMsg;
	std::vector<StdString> logMessages;

	//Clean up linefeeds for the logfile, all log messages should be single line.
	while (*str != '\0')
	{
		int i = 0;
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
	for (unsigned int i = 0; i < logMessages.size(); i++)
	{
		//if (TimeManager::GetInstance())
		//	m_CurrentLogSize += m_pLogFile->Print("[%15.6f\t] ", TimeManager::GetInstance()->GetCurrentTimeInSeconds());

		m_LogFile.Print(L("(%s) "), kLogCategories[in_eCategory]);
		m_LogFile.Print(L("(%s) "), kLogSeverities[in_eSeverity]);
		m_LogFile.Print(logMessages[i].c_str());
		m_LogFile.Print(L("\n"));
	}

	m_LogFile.Flush();
}


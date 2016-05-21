/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
 
#include <sstream>
#include <vector>
#include "meta.h"

#define LogDebug(str) _Log(___GETNAME(__LINE__)+str, LogEntry::Standard);

//star date 41153.7 - captain's log
struct LogEntry : public IMeta
{
	std::string text;
	enum LogEntryType
	{
		Standard, Command, ScriptOutput, Error, Exception
	};
	LogEntryType log_type;

	LogEntry() {}

	metadef(LogEntry)
		m_add(std::string, text)
		m_tag(MINTBOARDTAG)
		endmetadef
};

std::vector<LogEntry>* LogGetLogs(void);
extern unsigned logSize;

void LogLock();
void LogUnlock();

void LogClear(void);
void LogVerbose(std::string dataToLog, LogEntry::LogEntryType type = LogEntry::Standard);
void LogVerbose(Vector3 dataToLog, LogEntry::LogEntryType type = LogEntry::Standard);
void Log(std::string dataToLog, LogEntry::LogEntryType type = LogEntry::Standard);
void LogScript(std::string dataToLog);

//used to pass commands to the logger for parsing
void LogInterpret(std::string input);

/////////////////////////////////////
//Original Author: Nicholas Fuller
//Modifications:
//	Date	-	Change


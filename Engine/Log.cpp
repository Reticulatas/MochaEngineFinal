/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CScript.h"
#include "MemMgr.h"
#include "Config.h"
#include <stdio.h>
#include <time.h>
 
#include <windows.h>
 
//show verbose logs
#define VERBOSE

CRITICAL_SECTION logCritSection;

std::vector<LogEntry> logs;

unsigned logSize = 0;
static bool logLocked = false;
HANDLE hConsole = 0;

meta_define(LogEntry);

const std::string currentDateTime() {
	time_t now = time(0);
	struct tm  tstruct;
	char buf[80];
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%X", &tstruct);
	return buf;
}

#define SPAMTHRESHOLD 5
#define SPAMMSG "<< Omitted repeat messages >>"
void Log(std::string data, LogEntry::LogEntryType _type)
{
	static std::string lastLogData = "";
	static bool firstRun = false;
	static unsigned repeatedLogs = 0;
	if (!firstRun) {
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		InitializeCriticalSection(&logCritSection);
		firstRun = true;
	}
	if (logLocked)
		return;
	if (!Config::showConsole)
		return;
	//redirect to interpreter if the command is prefaced with a !
	if (data[0] == '!')
	{
		data = data.substr(1);
		LogInterpret(data);
		return;
	}
	//watch for repeat logs
	if (data != SPAMMSG)
	{
		if (data == lastLogData)
			repeatedLogs++;
		else
			repeatedLogs = 0;
		lastLogData = data;
		//stop broadcasting if this log is spamming
		if (repeatedLogs >= SPAMTHRESHOLD)
		{
			if (repeatedLogs == SPAMTHRESHOLD)
				Log(SPAMMSG);
			return;
		}
	}

	//thread safe logging
	EnterCriticalSection(&logCritSection);
	switch (_type)
	{
	case LogEntry::Command:
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN|FOREGROUND_INTENSITY);
		break;
	case LogEntry::ScriptOutput:
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
		break;
	case LogEntry::Exception:
	case LogEntry::Error:
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED|FOREGROUND_INTENSITY);
		break;

	}
	std::string d = "[" + currentDateTime() + "] ";
	d += data;
	std::cout << d << std::endl;
	LogEntry l;
	l.text = d;
	l.log_type = _type;
	logs.push_back(l);
	logSize++;
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	LeaveCriticalSection(&logCritSection);
}

void LogVerbose(std::string data, LogEntry::LogEntryType _type)
{
#ifdef VERBOSE
	Log(data, _type);
#endif
}

void LogVerbose(Vector3 data, LogEntry::LogEntryType _type)
{
//#ifdef VERBOSE
	Log("X : " + std::to_string(data.x) + " Y : " + std::to_string(data.y) + " Z : " + std::to_string(data.z) + "\n", _type);
//#endif
}


std::vector<LogEntry>* LogGetLogs( void )
{
	//quick check to make sure we aren't currently adding logs
	EnterCriticalSection(&logCritSection);
	LeaveCriticalSection(&logCritSection);
	return &logs;
}

void LogInterpret(std::string input)
{
	Log(input, LogEntry::Command);
	if (input == "ed sucks")
		Log("I agree", LogEntry::Error);
	if (input == "fuller sucks")
		Log("I disagree wholeheartedly", LogEntry::Error);
	if (input == "kai sucks")
		Log("Basic Physics Programmer", LogEntry::Error);

	if (input == "DebugOn")
	{
		Renderer::getInstance()->EnableDebugDraw(true);
		Log("Debug draw enabled", LogEntry::ScriptOutput);
	}
	if (input == "DebugOff")
	{
		Renderer::getInstance()->EnableDebugDraw(false);
		Log("Debug draw disabled", LogEntry::ScriptOutput);
	}
	if (input == "clear")
	{
		logs.clear();
		logSize = 0;
		Log("*** Logs Cleared ***");
	}
	if (input == "break")
	{
		assert(false);
	}
  if( input == "DumpMem")
  {
    MemMgr::getInstance()->DumpMemory();
  }
}

void LogScript( std::string dataToLog )
{
	Log(dataToLog, LogEntry::ScriptOutput);
}


void LogLock()
{
	logLocked = true;
}
void LogUnlock()
{
	logLocked = false;
}

/////////////////////////////////////
//Original Author: Nicholas Fuller
//Modifications:
//	Date	-	Change
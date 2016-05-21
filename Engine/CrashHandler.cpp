#include "stdafx.h"

#include "CrashHandler.h"
#include "StringUtils.h"

#include <Windows.h>
#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>

namespace CRASH_HANDLER
{
	LONG WINAPI WriteDump(EXCEPTION_POINTERS* pExceptionPtr);

	struct InfoForDump
	{
		EXCEPTION_POINTERS* m_pExceptionPtr;
		DWORD m_offendingThreadID;
	};
	DWORD WINAPI CreateDumpFile(void* info);
	void Init()
	{
		
		// Only works if there is no attached debugger
		// Otherwise, use __try __except(fn(GetExceptionInformation())) to write a minidump
		SetUnhandledExceptionFilter(WriteDump);

		//ULONG sizeOfStack = 4096*4;
		//SetThreadStackGuarantee(&sizeOfStack);
	}

	DWORD WINAPI CreateDumpFile(void* info)
	{
		InfoForDump* dumpInfo = (InfoForDump*)info;

		std::string timeStr = GetTimeStamp();
		std::wstring wTimeStr = std::wstring(timeStr.begin(), timeStr.end());
		std::wstring wDirectory(L"DumpFolder");
		std::wstring wFilename(L"dump_");
		wFilename += wTimeStr;
		wFilename += L".dmp";
		wFilename.insert(0, wDirectory + L"/");

		// std::wstring ss(L"Hello");
		CreateDirectory(wDirectory.c_str(), NULL);
		HANDLE hFile = CreateFile(wFilename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
		MINIDUMP_EXCEPTION_INFORMATION exceptionParam;
		exceptionParam.ThreadId = dumpInfo->m_offendingThreadID;
		exceptionParam.ExceptionPointers = dumpInfo->m_pExceptionPtr; // Comes from the exception
		exceptionParam.ClientPointers = false; // Used when writing a dump for different process

		MINIDUMP_TYPE smallDump = (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithDataSegs | 
			MiniDumpWithCodeSegs | MiniDumpWithIndirectlyReferencedMemory);
		MINIDUMP_TYPE everythingDump = (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithFullMemory |
			MiniDumpWithHandleData | MiniDumpWithUnloadedModules);
		bool useEverythingDump = false;

		BOOL result = MiniDumpWriteDump(
			GetCurrentProcess(), // Process handle
			GetCurrentProcessId(), // Process ID
			hFile, // File to write to
			useEverythingDump ? everythingDump : smallDump,
			&exceptionParam,
			NULL, // UserStream, write whatever data you want like name of player or which world currently in
			NULL); // CallbackParam, customize settings of what to write to mini dump

		return EXCEPTION_CONTINUE_SEARCH; // Allow an attached debugger to receive the exception
	}
	LONG WINAPI WriteDump(EXCEPTION_POINTERS* pExceptionPtr)
	{
		InfoForDump info;
		info.m_offendingThreadID = GetCurrentThreadId();
		info.m_pExceptionPtr = pExceptionPtr;

		HANDLE hThread = CreateThread(
			NULL, // LPSECURITY_ATTRIBUTES lpThreadAttributes,
			0, // SIZE_T dwStackSize,
			CreateDumpFile, // LPTHREAD_START_ROUTINE lpStartAddress,
			&info, // LPVOID lpParameter,
			0, // DWORD dwCreationFlags, // 0 means starts immediately
			NULL); // LPDWORD lpThreadId
		if (hThread == NULL)
		{
			// Not able to create thread, then try to call regularly anyway
			return (LONG)CreateDumpFile(&info);
		}

		WaitForSingleObject(hThread, INFINITE); // Wait for the thread to finish writing to dump file

		DWORD returnValue;
		GetExitCodeThread(hThread, &returnValue); // Get the return value

		CloseHandle(hThread);
		
		return (LONG)returnValue; // Allow an attached debugger to receive the exception
	}
}

/////////////////////////////////////
//Original Author: Elroy Chan
//Modifications:
//	Date	-	Change
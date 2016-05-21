/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
 
namespace ThreadUtils
{
	const char SECTIONIDNAMESIZE = 20;
	struct SectionID 
	{
		char id;
		char sectionName[SECTIONIDNAMESIZE];
	};

	//Critical Sections-----------
	const char NUMOFSECTIONS = 8;
  SectionID sectionIDs[NUMOFSECTIONS] = { { 0, "Main" }, { 1, "Editor" }, { 2, "MissionCritical" }, { 3, "MessageManager" }, { 4, "FrustrumCulling" }, { 5, "ThreadedQueue" }, { 6, "RenderOculus" }, { 7, "RenderOculus" } };
	//----------------------------
	CRITICAL_SECTION main_CriticalSections[ThreadUtils::NUMOFSECTIONS];

	ThreadLockVisitor::~ThreadLockVisitor()
	{
		ThreadUtils::LeaveCritSection(myId);
	}

	ThreadLockVisitor::ThreadLockVisitor( char id ) : myId(id)
	{
		ThreadUtils::EnterCritSection(id);
	}

	bool IsCritSectionInUse( char id /*= 0*/ )
	{
		if (TryEnterCriticalSection(&main_CriticalSections[id]) == 0)
			return true;
		else
			LeaveCriticalSection(&main_CriticalSections[id]);
		return false;
	}


	ThreadTryLockVisitor::ThreadTryLockVisitor( char id )
	{
		myId = id;
		if (IsCritSectionInUse(id))
			mDidLock = false;
		ThreadUtils::EnterCritSection(id);
		mDidLock = true;
	}

	ThreadTryLockVisitor::~ThreadTryLockVisitor()
	{
		if (GetDidLock())
			ThreadUtils::LeaveCritSection(myId);
	}

	bool ThreadTryLockVisitor::GetDidLock()
	{
		return mDidLock;
	}

  void SetThreadName(std::thread* thread, LPCSTR name)
  {
    DWORD ThreadId = ::GetThreadId(static_cast<HANDLE>(thread->native_handle()));

    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = name;
    info.dwThreadID = ThreadId;
    info.dwFlags = 0;

    __try
    {
      RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
    }
  }

}

void ThreadUtils::Startup(void)
{
	for (int i = 0; i < ThreadUtils::NUMOFSECTIONS; i++)
		InitializeCriticalSection(&(main_CriticalSections[i]));
}

void ThreadUtils::EnterCritSection( char id )
{
	//if you broke here, see top of this file
	assert (id < NUMOFSECTIONS);
	EnterCriticalSection(&main_CriticalSections[id]);
}

void ThreadUtils::Cleanup( )
{
	for (int i = 0; i < ThreadUtils::NUMOFSECTIONS; i++)
		DeleteCriticalSection(&main_CriticalSections[i]);
}

void ThreadUtils::LeaveCritSection( char id )
{
	LeaveCriticalSection(&main_CriticalSections[id]);
}


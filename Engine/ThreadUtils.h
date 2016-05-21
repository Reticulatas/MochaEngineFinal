/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
  DWORD dwType; // Must be 0x1000.
  LPCSTR szName; // Pointer to name (in user addr space).
  DWORD dwThreadID; // Thread ID (-1=caller thread).
  DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

namespace ThreadUtils
{
	void Startup(void);
	void EnterCritSection(char id = 0);
	void LeaveCritSection(char id = 0);
	//this function should not be called from a thread that could possibly contain the thread being checked
	//it will briefly enter the crit section is it is NOT in use, but exit immediately
	bool IsCritSectionInUse(char id = 0);
	void Cleanup(void);

	class ThreadLockVisitor 
	{
		char myId;
	public:		
		ThreadLockVisitor(char id);
		~ThreadLockVisitor();
	};
	class ThreadTryLockVisitor 
	{
		char myId;
		bool mDidLock;
	public:		
		ThreadTryLockVisitor(char id);
		~ThreadTryLockVisitor();
		bool GetDidLock();
	};

  void SetThreadName(std::thread* thread, LPCSTR name);
}
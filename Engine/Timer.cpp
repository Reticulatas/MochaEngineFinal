/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include <mmsystem.h>

Timer::Timer() : mStartTime(0), mPauseTime(0), mIsRunning(false), mIsPaused(false)
{
}

void Timer::Start()
{
	mIsRunning = true;
	mIsPaused = false;

	mStartTime = timeGetTime();
}

void Timer::Stop()
{
	mIsRunning = false;
	mIsPaused = false;
}

double Timer::GetTime()
{
	if(mIsRunning == true)
	{
		if(mIsPaused == true)
			return mPauseTime;

		else
			return timeGetTime() - mStartTime;
	}

	return 0;
}

void Timer::Pause()
{
	if(mIsRunning == true && mIsPaused == false)
	{
		mIsPaused = true;
		mPauseTime = timeGetTime() - mStartTime;
	}
}

void Timer::Resume()
{
	if(mIsPaused == true)
	{
		mIsPaused = false;
		mStartTime = timeGetTime() - mPauseTime;
		mPauseTime = 0;
	}
}

bool Timer::IsRunning()
{
	return mIsRunning;
}

bool Timer::IsPaused()
{
	return mIsPaused;
}

//-----

int Bob(float, char)
{
	return 5;
}

struct TimerData { unsigned interval; void (*fnc)(void); };
DWORD WINAPI T_TimerThread(void * params)
{
	TimerData* td = (TimerData*)params;
	while (true)
	{
		Sleep(td->interval);
		td->fnc();
	}
	return 0;
}


//Interval in ms
ManagedTimer::ManagedTimer( unsigned _int, void (*_fnc)(void) )
{
	TimerData td = { _int, _fnc };
	tHandle = CreateThread(0,0, T_TimerThread,&td,0,0);
}

ManagedTimer::~ManagedTimer()
{
	TerminateThread(tHandle, 0);
	CloseHandle(tHandle);
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
//	11/5/13 -	Timer Class Methods

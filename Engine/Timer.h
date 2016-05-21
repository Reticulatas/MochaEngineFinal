/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include <Windows.h>
#pragma comment(lib, "WINMM.lib")

class Timer
{
	public:
		//Constructor
		Timer();
		Timer(float interval, void (*func)(void));

		//Timer functions
		void Start();
		void Stop();
		void Pause();
		void Resume();

		//Gets the timer's time
		double GetTime();

		//Status of timers
		bool IsRunning();
		bool IsPaused();

	private:
		double mStartTime;
		double mPauseTime;
		bool mIsRunning;
		bool mIsPaused;
};

class ManagedTimer : public ::Timer
{
	unsigned mInterval;
	void (*mFunc)(void);
	HANDLE tHandle;

public:
	ManagedTimer(unsigned _int, void (*_fnc)(void));
	~ManagedTimer();

};

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
//  11/5/13 -   Timer Class
//  1/8/13	-	Added ability for 
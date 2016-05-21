/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
 
#include <stdio.h>

FrameController::FrameController()
{
	mFPSMax = 0;
	mCap = false;
	mFrameRate.Start();
	mStartTime = mEndTime = (float)mFrameRate.GetTime();
	mDeltaTime = 0;
  mGlobalTime = 0;
}

bool FrameController::Tick()
{
	//reconfirm delta times after the loop exits
	mEndTime = (float)mFrameRate.GetTime();
	if(mCap)
	{
		mDeltaTime = (mEndTime - mStartTime) * .001f;	//how long the frame took
		float mTargetDeltaTime = 1.0f/mFPSMax;

		//throw away super-low fps
		if (mDeltaTime > (1.0f / 5.0f))
		{
			Log("Bad Frame!", LogEntry::Error);
			//mDeltaTime = 0;
		}

		float mDeltaTimeToWaste = mTargetDeltaTime - mDeltaTime;
		if (mDeltaTimeToWaste > 0.0f)
		{
			do 
			{
				//Sleep((DWORD)(mDeltaTimeToWaste*1000.0f));
				mEndTime = (float)mFrameRate.GetTime();
				mDeltaTime = (mEndTime - mStartTime) * .001f;
			}
			while (mDeltaTime < mTargetDeltaTime);
		}
	}
	
	//std::cout << mDeltaTime << std::endl;
	mFPS = 1 / mDeltaTime;
	mStartTime = mEndTime;
  mGlobalTime += mDeltaTime;
  if (Equal(mGlobalTime, std::numeric_limits<float>::max()))
    mGlobalTime = 0;

	return true;
}


float FrameController::GetDeltaTime()
{
	return mDeltaTime;
}

void FrameController::CapOn(float fps)
{
	mCap = true;
	mFPSMax = fps;
}
void FrameController::CapOff()
{
	mCap = false;
}

FrameController* FrameController::getInstance()
{
	static FrameController* instance = 0;

	if(!instance)
		instance = new FrameController();

	return instance;
}

void FrameController::BeginTimeSample( std::string name )
{
#ifdef _DEBUG
	//register sample begin
	timeSamples[name] = (float)mFrameRate.GetTime();
#endif
}

void FrameController::EndTimeSample( std::string name, bool outputSample )
{
#ifdef _DEBUG
	//register sample end
	float sample = ((float)mFrameRate.GetTime() - timeSamples[name]) * 0.001f;
	timeSamples[name] = sample;
	if (outputSample)
		std::cout << "\tTIME SAMPLE [" << name << " = " << sample << "]" << std::endl;
#endif
}

float FrameController::GetLastTimeSample( std::string name )
{
	if (timeSamples.find(name) == timeSamples.end())
		return 0xFF;
	return timeSamples[name];
}

float FrameController::DT()
{
	return getInstance()->GetDeltaTime();
}

float FrameController::GetGlobalTime()
{
  return mGlobalTime;
}

meta_define(FrameController);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
//	12/5/13 -	FrameController Class Methods
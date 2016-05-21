/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
 
#include <map>
#include "ISubsystem.h"
#include "Timer.h"

 
class FrameController : public ISubsystem
{
public:
	/*Constructor*/
	FrameController();
	
	bool Tick();
	/*! \brief get delta time in seconds */
	float GetDeltaTime();
  float GetGlobalTime();
	void CapOn(float fps);
	void CapOff();
	float FPSMax() const { return mFPSMax; }

	void BeginTimeSample(std::string name);
	void EndTimeSample(std::string name, bool outputTimeSample = false );
	float GetLastTimeSample(std::string name);

	static FrameController* getInstance(); // singleton
	static float DT();

private:
	::Timer mFrameRate;	//Timer
	float mFPSMax;		//60fps
	float mFPS;			//Current FPS
	float mStartTime;	//Start of game loop
	float mEndTime;		//End of game loop
	float mDeltaTime;	//dt
  float mGlobalTime;
	bool mCap;			//Cap on or off

	//generic performance data sampling table
	std::map<std::string, float> timeSamples;
	typedef std::map<std::string, float>::iterator timeSamples_iter;

	friend class StateTabContent;
	friend class PerfVisualizerComponent;

	metadef(FrameController)
		endmetadef;
};
 


/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
//	12/5/13	-	FrameController Class
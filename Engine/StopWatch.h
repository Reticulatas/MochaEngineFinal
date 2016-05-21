/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
class Stopwatch
{
private:

	float   m_performanceFrequency;
	bool    m_started;
	__int64 m_timeStarted;

public:

	Stopwatch(void);

	/*!
	@brief
	Starts the stop watch.
	*/
	void Start(void);

	/*!
	@brief
	Stops the stop watch and checks how much time has elapsed since it
	was started.
	@return
	Time elapsed since last start in seconds.
	*/
	float Stop(void);
};
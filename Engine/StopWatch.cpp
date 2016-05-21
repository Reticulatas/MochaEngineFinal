#include "stdafx.h"
#include "StopWatch.h"
Stopwatch::Stopwatch(void)
: m_started(false)
{
	__int64 countsPerSecond = 0;
	QueryPerformanceFrequency((LARGE_INTEGER *)&countsPerSecond);
	m_performanceFrequency = 1.0f / static_cast<float>(countsPerSecond);
}

void Stopwatch::Start(void)
{
	if (m_started) return;

	QueryPerformanceCounter((LARGE_INTEGER *)&m_timeStarted);
	m_started = true;
}

float Stopwatch::Stop(void)
{
	if (!m_started) return 0.0f;

	__int64 timeNow_ = 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&timeNow_);

	m_started = false;

	return m_performanceFrequency * static_cast<float>(timeNow_ - m_timeStarted);
}
#include "Platform/Timer.hpp"

#include <Windows.h>

namespace Charra
{
	namespace Timer
	{
		double g_startTime;
		double g_millisecondConversion;

		LARGE_INTEGER g_frequency;

		void initTimer()
		{
			QueryPerformanceFrequency(&g_frequency);
			g_millisecondConversion = 1000.0f / g_frequency.QuadPart;
		}

		double getTimeSinceInit()
		{
			return getCurrentTime() - g_startTime;
		}
		
		double getCurrentTime()
		{
			LARGE_INTEGER time;
			QueryPerformanceCounter(&time);
			return time.QuadPart * g_millisecondConversion;
		}

		double millisecondsToSeconds(double milliseconds)
		{
			return milliseconds / 1000.0f;
		}

		void wait(double desiredTime)
		{
			double end = getCurrentTime() + desiredTime;
			
			// the 3 is to try and account for the microsoft scheduler which updates every 4ms
			while(getCurrentTime() < end - 3)
			{
				Sleep(1);
			}
			while(getCurrentTime() < end)
			{
				Sleep(0);
			}
		}
	}

	ScaledTimer::ScaledTimer(float scaleFactor)
	: m_scaleFactor(scaleFactor)
	{
		m_startTime = Timer::getCurrentTime();
		m_lastCheckedTime = m_startTime;
	}

	ScaledTimer::ScaledTimer()
	: m_scaleFactor(1.0f)
	{
		m_startTime = Timer::getCurrentTime();
		m_lastCheckedTime = m_startTime;
	}

	ScaledTimer::~ScaledTimer()
	{

	}

	double ScaledTimer::getTimeDiff(double desiredDiff)
	{
		desiredDiff *= m_scaleFactor;


		if(desiredDiff == 0.0f)
		{
			double now = Timer::getCurrentTime();
			double timeDiff = now - m_lastCheckedTime;
			m_lastCheckedTime = now;	

			return timeDiff;
		}

		double timeDiff = Timer::getCurrentTime() - m_lastCheckedTime;

		Timer::wait(desiredDiff - timeDiff);

		double now = Timer::getCurrentTime();
		timeDiff = now - m_lastCheckedTime;
		m_lastCheckedTime = now;

		return timeDiff;
	}

	void ScaledTimer::waitForTime(double desiredDiff)
	{
		desiredDiff *= m_scaleFactor;
		Timer::wait(desiredDiff);
	}
}
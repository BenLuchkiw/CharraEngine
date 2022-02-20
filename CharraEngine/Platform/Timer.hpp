#pragma once

namespace Charra
{
	namespace Timer
	{
		void initTimer();

		// Returns value in ms
		double getTimeSinceInit();
		// Returns value in ms
		double getCurrentTime();

		double millisecondsToSeconds(double milliseconds);

		void wait(double desiredTime);
	}

	class ScaledTimer
	{
		public:
			ScaledTimer(float scaleFactor);
			ScaledTimer();
			~ScaledTimer();

			inline void updateScale(float scaleFactor) { m_scaleFactor = scaleFactor; }
			inline float getScale() { return m_scaleFactor; }
			inline double getTimeSinceInit() { return m_startTime; }
			
			// If desired time is non-zero the function will halt the thread until desired time has elapsed,
			// This will update give the time since this function was last called, or since the constructor
			// It will follow the scale factor
			double getTimeDiff(double desiredDiff);

			// The thread will halt until desired time has elapsed, it will follow the scale factor
			void waitForTime(double desiredDiff);
		private: // Methods

		private: // Methods
			float m_scaleFactor = 1.0f;
			double m_startTime;
			double m_lastCheckedTime;
	};
}
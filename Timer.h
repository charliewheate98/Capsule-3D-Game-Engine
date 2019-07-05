#ifndef TIMER_H
#define TIMER_H

#include <Query.h>

class Timer {

private:
	unsigned __int64 baseTime_;
public:
	double freq_;
	Timer(double speed) { reset(speed); }

	unsigned __int64 GetBaseTime() { return baseTime_; }

	// reset() makes the timer start over counting from 0.0 seconds.
	void reset(double speed)
	{
		unsigned __int64 pf;
		QueryPerformanceFrequency((LARGE_INTEGER *)&pf);
		freq_ = speed / (double)pf;
		QueryPerformanceCounter((LARGE_INTEGER *)&baseTime_);
	}

	// seconds() returns the number of seconds (to very high resolution)
	// elapsed since the timer was last created or reset().
	double seconds()
	{
		unsigned __int64 val;
		QueryPerformanceCounter((LARGE_INTEGER *)&val);
		return (val - baseTime_) * freq_;
	}

	// seconds() returns the number of milliseconds (to very high resolution)
	// elapsed since the timer was last created or reset().
	double milliseconds() { return seconds() * 1000.0; }
};

#endif
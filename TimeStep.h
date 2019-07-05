#ifndef __TIME_STEP_COUNTER_H__
#define __TIME_STEP_COUNTER_H__

#include <iostream>
#include <Query.h>	// Include query for high-res counting
#include "Globals.h"	// Access our global variables


// This class will be responsible for maintaining a fixed fps, also known as "timestep"
class TimeStepCounter
{
private:
	LARGE_INTEGER	_frequency;		// Get the freguency of the current time
	LARGE_INTEGER	_last;			// Get the last time (elapsed time)
	LARGE_INTEGER	_current;		// The current time

public:
	double			_delta;			// The tick rate (this variable is public as it MUST be an lvalue for parsing)

	// Initialise the our perfomance counter by getting the frequency
	inline void Initialise()
	{
		QueryPerformanceFrequency(&_frequency);
		QueryPerformanceCounter(&_last);
	}

	// Analyse our performance counter by performing delta calculation
	inline void Analyse()
	{
		QueryPerformanceCounter(&_current);
		_delta = ((double)(_current.QuadPart - _last.QuadPart) / (double)_frequency.QuadPart) * _rt_speed;
	}

	// Reset the time back to current
	inline void Reset()
	{
		_last = _current;
	}
};

#endif
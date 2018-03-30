#include "ISEngine.h"

#ifndef _WIN32
timespec diff(timespec start, timespec end)
{
	timespec temp;
	if((end.tv_nsec - start.tv_nsec) < 0)
	{
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	}
	else
	{
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}
	return temp;
}
#endif

//-----------------------------------------------------------------------------
// Name: Time()
// Desc: Constructor
//-----------------------------------------------------------------------------
Engine::Time::Time()
{
#ifdef _WIN32
	QueryPerformanceCounter(&framestart);
	QueryPerformanceFrequency(&freq);
#else
	clock_gettime(CLOCK_REALTIME, &framestart);
#endif
	dt = 0.0001f;
	t = 0.0f;
	frameratefixed = false;
	timefactor = 1.0f;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Update t (absolute time) and dt (delta time)
//-----------------------------------------------------------------------------
void Engine::Time::Step()
{
	if(frameratefixed)
		dt += invfps;
	else
	{
#ifdef _WIN32
		LARGE_INTEGER frameend;
		QueryPerformanceCounter(&frameend);
		dt = (float)(frameend.QuadPart - framestart.QuadPart) / ((float)freq.QuadPart * timefactor);
		t += dt;
		framestart.QuadPart = frameend.QuadPart;
#else
		timespec frameend;
		clock_gettime(CLOCK_REALTIME, &frameend);
		timespec frametime = diff(framestart, frameend);
		dt = (float)frametime.tv_sec + (float)frametime.tv_nsec / 1000000000.0f;
		t += dt;
		framestart = frameend;
#endif
	}
}

//-----------------------------------------------------------------------------
// Name: Configure()
// Desc: Process one frame and return false to initiate application termination
// Params:
//		 frameratefixed == true => value ... [frames/second]
//		 frameratefixed == false => value ... time factor (for slow motion)
//-----------------------------------------------------------------------------
void Engine::Time::Configure(bool frameratefixed, float value)
{
	this->frameratefixed = frameratefixed;

	if(frameratefixed)
		invfps = 1.0f / value;
	else
		timefactor = value;
}
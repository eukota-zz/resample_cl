#pragma once
#include <iostream>
#include <vector>



#include <Windows.h> ///< for performance counters

struct ProfilerStruct
{
	ProfilerStruct() { CountStart.QuadPart = 0; CountLap.QuadPart = 0; CountStop.QuadPart = 0; }
	LARGE_INTEGER CountStart;
	LARGE_INTEGER CountLap;
	LARGE_INTEGER CountStop;
	void Start();
	float Lap();
	void Stop();
	LARGE_INTEGER AcquireFrequency();
	float Log(bool writeToLog = false);
	float GetTimeDiff(LARGE_INTEGER start, LARGE_INTEGER stop);
};

#include <vector>
#include "profiler.h"
#include "utils.h"

using namespace std;

// Start Profiler
void ProfilerStruct::Start()
{
	QueryPerformanceCounter(&CountStart);
	CountLap = CountStart;
}

// Record and Continue
float ProfilerStruct::Lap()
{
	LARGE_INTEGER start = CountLap;
	QueryPerformanceCounter(&CountLap);
	return GetTimeDiff(start, CountLap);
}

// Stop Profiler
void ProfilerStruct::Stop()
{
	QueryPerformanceCounter(&CountStop);
}

// Acquire Profiler Frequency
LARGE_INTEGER ProfilerStruct::AcquireFrequency()
{
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	return Frequency;
}

// Print Profiler Log
float ProfilerStruct::Log(bool writeToLog)
{
	float runTime = GetTimeDiff(CountStart, CountStop);
	if (writeToLog)
		LogInfo("Performance Counter Time %f ms.\n", runTime);
	return runTime;
}

float ProfilerStruct::GetTimeDiff(LARGE_INTEGER start, LARGE_INTEGER stop)
{
	LARGE_INTEGER freq = AcquireFrequency();
	return (1000.0f*(float)(stop.QuadPart - start.QuadPart) / (float)freq.QuadPart);
}
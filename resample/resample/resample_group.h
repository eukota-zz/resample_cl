#pragma once
#include "CL/cl.h"
#include "groups.h"


class ResampleGroup : public GroupManager
{
public:
	ResampleGroup();
	~ResampleGroup();

	std::map<int, ProblemGroup*> GroupFactory();
};
extern ResampleGroup* ResampleGroupObject;

// Full Reseample Functions
cl_float* ResampleOcl(cl_float* signalInput, cl_float* QTranspose, cl_float* R, size_t sampleInputCount, size_t sampleOutputCount, size_t sampleOrder, cl_float* outputTimes, ResultsStruct* results);
int Run_ResampleOcl(ResultsStruct* results);



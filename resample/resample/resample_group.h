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
cl_float* ResampleOcl(const std::string& inputFile, size_t inputRate, size_t outputRate, size_t order, cl_float** coeffs, bool verbose);
int Run_ResampleOcl(ResultsStruct* results);



#include "groups.h"
#include "resample_group.h"
#include "sequential.h"
#include "parallel.h"

ResampleGroup* ResampleGroupObject;

ResampleGroup::ResampleGroup()
	: GroupManager("Resample")
{
	groups_ = GroupFactory();
}

ResampleGroup::~ResampleGroup()
{
}

std::map<int, ProblemGroup*> ResampleGroup::GroupFactory()
{
	std::map<int, ProblemGroup*> pgs;

	// Various Input Modification Functions
	ProblemGroup* InputControl = GroupManagerInputControlFactory();
	pgs[InputControl->GroupNum()] = InputControl;

	// Resample Run Functions
	size_t idx = 0; 
	ProblemGroup* resampleRelease = new ProblemGroup(1, "Resample Run");
	resampleRelease->problems_[++idx] = new Problem(&Run_ResampleOcl, "Run Complete Resample using OpenCL");
	pgs[resampleRelease->GroupNum()] = resampleRelease;
	return pgs;
}

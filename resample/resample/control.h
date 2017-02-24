#pragma once
#include "groups.h"


class ControlClass : public GroupManager
{
public:
	ControlClass();
	virtual std::string ProblemGroupName() { return "Control"; }
	virtual std::string ProblemName() { return ""; }

	std::map<int, ProblemGroup*> GroupFactory();
};

int exCL_ProgressiveArraySum(ResultsStruct* results);
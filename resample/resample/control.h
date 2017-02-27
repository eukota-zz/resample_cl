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

int exCL_Resample(ResultsStruct* results);
int exSeq_Resample(ResultsStruct* results);

int exCL_QRD(ResultsStruct* results);
int exSeq_QRD(ResultsStruct* results);

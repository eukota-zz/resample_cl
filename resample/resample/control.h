#pragma once
#include "groups.h"


class ControlClass : public GroupManager
{
public:
	ControlClass();
	~ControlClass()
	{
		free(sample_data_);
	}
	virtual std::string ProblemGroupName() { return "Control"; }
	virtual std::string ProblemName() { return ""; }
	std::map<int, ProblemGroup*> GroupFactory();

	int LoadSampleData(bool printPoints = false);
	float* sample_data_;
	int sample_rate_input_;
	int sample_rate_output_;
	std::string sample_data_input_file_;
};
extern ControlClass* ControlObject;

int SetSampleRates(ResultsStruct* results);
int SetInputDataFile(ResultsStruct* results);
int readSampleDataTest(ResultsStruct* results);



int exCL_Resample(ResultsStruct* results);
int exSeq_Resample(ResultsStruct* results);

int exCL_QRD(ResultsStruct* results);
int exSeq_QRD(ResultsStruct* results);

#pragma once
#include "groups.h"


class ControlClass : public GroupManager
{
public:
	ControlClass();
	~ControlClass();

	virtual std::string ProblemGroupName() { return "Control"; }
	virtual std::string ProblemName() { return ""; }
	std::map<int, ProblemGroup*> GroupFactory();

	// Sample Data
	int LoadSampleData(bool printPoints = false);
	float* sample_data_;
	int sample_rate_input_;
	int sample_rate_output_;
	std::string sample_data_input_file_;

	// QRD
	int* Q;
	int* R;
};
extern ControlClass* ControlObject;

int SetSampleRates(ResultsStruct* results);
int SetInputDataFile(ResultsStruct* results);
int Test_LoadSampleData(ResultsStruct* results);



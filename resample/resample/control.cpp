#include "CL/cl.h"
#include "ocl.h"
#include "tools.h"
#include "utils.h"
#include "data.h"
#include "control.h"
#include "profiler.h"
#include "enums.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "windows.h"
#include "shlwapi.h"
#include "parallel.h"
#include "sequential.h"
#include <fstream>

ControlClass::ControlClass()
	: GroupManager("Control")
	, sample_data_(NULL)
	, sample_rate_input_(1000)
	, sample_rate_output_(500)
	, sample_data_input_file_("..\\data\\default.csv")
{
	groups_ = GroupFactory();
}
ControlClass* ControlObject;

std::map<int, ProblemGroup*> ControlClass::GroupFactory()
{
	std::map<int, ProblemGroup*> pgs;

	ProblemGroup* InputControl = GroupManagerInputControlFactory();
	InputControl->problems_[InputControl->problems_.size() + 1] = new Problem(&SetInputDataFile, "Set the file path to read sample data from.");
	InputControl->problems_[InputControl->problems_.size() + 1] = new Problem(&SetSampleRates, "Set the input and output sample rates.");
	InputControl->problems_[InputControl->problems_.size() + 1] = new Problem(&readSampleDataTest, "Read and print sample data.");
	pgs[InputControl->GroupNum()] = InputControl;

	ProblemGroup* projectFuncs = new ProblemGroup(1, "Control");
	projectFuncs->problems_[projectFuncs->problems_.size() + 1] = new Problem(&exCL_Resample, "OpenCL: Apply sixth-order polynomial");
	projectFuncs->problems_[projectFuncs->problems_.size() + 1] = new Problem(&exSeq_Resample, "Sequental: Apply sixth-order polynomial");
	projectFuncs->problems_[projectFuncs->problems_.size() + 1] = new Problem(&exSeq_QRD, "Sequental: QRD");
	pgs[projectFuncs->GroupNum()] = projectFuncs;
	return pgs;
}

// Attempts to read sample input data from provided data file
// Expects to find one amplitude value per line
// @return the number of data points read
int ControlClass::LoadSampleData(bool printPoints)
{
	// With help from:
	//http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
	std::ifstream f(ControlObject->sample_data_input_file_.c_str());
	if (!f.good())
		return -1;
	int pointCount = 0;
	std::filebuf fb;
	std::vector<float> points;
	if (fb.open(ControlObject->sample_data_input_file_, std::ios::in))
	{
		std::istream is(&fb);
		char point[256];
		while (is.getline(point, 256))
			points.push_back(std::stof(point));
		fb.close();
	}
	if (printPoints)
		std::cout << "Read Points: " << std::endl;
	sample_data_ = (float*)malloc(sizeof(float*)*points.size());
	for (size_t i = 0; i < points.size(); ++i)
	{
		sample_data_[i] = points[i];
		if(printPoints)
			std::cout << i << ": " << points[i] << std::endl;
	}

	return points.size();
}

// Set the Input and Output sample rates
int SetSampleRates(ResultsStruct* results)
{
	std::cout << "Enter INPUT sample rate (currently " << ControlObject->sample_rate_input_ << "): ";
	std::cin >> ControlObject->sample_rate_input_;
	std::cout << "Enter OUTPUT sample rate (currently " << ControlObject->sample_rate_output_ << "): ";
	std::cin >> ControlObject->sample_rate_output_;
	return 0;
}

// Set Input Data File path
int SetInputDataFile(ResultsStruct* results)
{
	std::cout << "Enter path to input sample file to (currently " << ControlObject->sample_data_input_file_ << "): ";
	std::string s(ControlObject->sample_data_input_file_);
	std::cin >> s;
	ControlObject->sample_data_input_file_ = s;
	std::ifstream f(ControlObject->sample_data_input_file_.c_str());
	if (!f.good())
	{
		std::cout << "WARNING: File does not exist: " << ControlObject->sample_data_input_file_ << std::endl;
		return -1;
	}
	return 0;
}

// Reads sample data and prints it to the screen
// Uses LoadSampleData() function with optional print bool set to true
int readSampleDataTest(ResultsStruct* results)
{
	// returns number of loaded
	bool printPoints = true; // for test purposes
	int loadCount = ControlObject->LoadSampleData(printPoints);
	std::cout << "Points Loaded: " << loadCount << std::endl;
	return (loadCount >= 0 ? 0 : -1);
}


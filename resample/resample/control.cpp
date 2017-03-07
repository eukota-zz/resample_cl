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

ControlClass* ControlObject;

ControlClass::ControlClass()
	: GroupManager("Control")
	, sample_data_(NULL)
	, sample_rate_input_(1000)
	, sample_rate_output_(500)
	, sample_data_input_file_("..\\data\\default.csv")
{
	groups_ = GroupFactory();
}

ControlClass::~ControlClass()
{
	free(sample_data_);
}

std::map<int, ProblemGroup*> ControlClass::GroupFactory()
{
	std::map<int, ProblemGroup*> pgs;

	ProblemGroup* InputControl = GroupManagerInputControlFactory();
	size_t idx = InputControl->problems_.size();
	InputControl->problems_[++idx] = new Problem(&SetInputDataFile, "Set the file path to read sample data from.");
	InputControl->problems_[++idx] = new Problem(&SetSampleRates, "Set the input and output sample rates.");
	InputControl->problems_[++idx] = new Problem(&Test_LoadDataFile, "Read and print sample data.");
	pgs[InputControl->GroupNum()] = InputControl;

	idx = 0; // reset counter
	ProblemGroup* projectFuncs = new ProblemGroup(1, "Control");
	projectFuncs->problems_[++idx] = new Problem(&exCL_Resample, "OpenCL: Apply sixth-order polynomial");
	projectFuncs->problems_[++idx] = new Problem(&Test_PolyEval, "Test Polynomial Evaluation Function");
	projectFuncs->problems_[++idx] = new Problem(&Test_QR, "Test QR Decomposition Function");
	projectFuncs->problems_[++idx] = new Problem(&Test_BackSub, "Test Back Substitution Function");
	projectFuncs->problems_[++idx] = new Problem(&Test_CreateIdentityMatrix, "Test Create Identity Matrix Function");
	projectFuncs->problems_[++idx] = new Problem(&Test_SignalGenerator, "Test Signal Generator Function");
	projectFuncs->problems_[++idx] = new Problem(&Test_TransposeMatrix, "Test Transpose Matrix Function");
	projectFuncs->problems_[++idx] = new Problem(&Test_GenerateAMatrix, "Test Generator A Matrix Function");
	projectFuncs->problems_[++idx] = new Problem(&Test_CopyMatrix, "Test Copy Matrix Function");
	projectFuncs->problems_[++idx] = new Problem(&Test_MatrixMultiplier, "Test Matrix Mutlipication Function");
	projectFuncs->problems_[++idx] = new Problem(&Test_Resample, "Test Complete Resample Function");
	pgs[projectFuncs->GroupNum()] = projectFuncs;
	return pgs;
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


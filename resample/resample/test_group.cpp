#include "CL/cl.h"
#include "ocl.h"
#include "tools.h"
#include "utils.h"
#include "data.h"
#include "test_group.h"
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

TestGroup* TestGroupObject;

TestGroup::TestGroup()
	: GroupManager("Test")
{
	groups_ = GroupFactory();
}

TestGroup::~TestGroup()
{
}

std::map<int, ProblemGroup*> TestGroup::GroupFactory()
{
	std::map<int, ProblemGroup*> pgs;

	// Various Input Modification Functions
	ProblemGroup* InputControl = GroupManagerInputControlFactory();
	pgs[InputControl->GroupNum()] = InputControl;

	// Sequential Code Test Functions
	size_t idx = 0;
	ProblemGroup* projectFuncs = new ProblemGroup(1, "Sequential Tests");
	projectFuncs->problems_[++idx] = new Problem(&Test_LoadDataFile, "Test Load Data File Function");
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

	// Parallel Code Test Functions
	idx = 0; // reset counter
	ProblemGroup* projectFuncsOpenCL = new ProblemGroup(2, "OpenCL Tests");
	projectFuncsOpenCL->problems_[++idx] = new Problem(&Test_MatrixMultiplierOcl, "Test OpenCL Matrix Multiplication Function");
	projectFuncsOpenCL->problems_[++idx] = new Problem(&Test_PolyEvalOcl, "Test OpenCL Polynomial Evaluation Function");
	pgs[projectFuncsOpenCL->GroupNum()] = projectFuncsOpenCL;
	return pgs;
}

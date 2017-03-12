#include "groups.h"
#include "resample_group.h"
#include "sequential.h"
#include "parallel.h"
#include "settings.h"
#include "profiler.h"
#include "tools.h"
#include <iostream>
#include <stack>

//#define VERBOSE_FILE_SAVE

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

// Complete Resample in OpenCL
cl_float* ResampleOcl(cl_float* signalInput, cl_float* QTranspose, cl_float* R, size_t sampleInputCount, size_t sampleOutputCount, size_t sampleOrder, cl_float* outputTimes, ResultsStruct* results)
{
	tools::FreeMemoryFinalizer freeDeferred;
	ProfilerStruct profiler;
	profiler.Start();

	// OpenCL Multiply QTranspose by SignalData
	(void)profiler.Lap();
	cl_float* Qtb = MatrixMultiplierOcl(QTranspose, sampleInputCount, sampleInputCount, signalInput, sampleInputCount, 1);
	results->AddRunTime(profiler.Lap(), "MatrixMultiplierOcl Time");
	if (!Qtb)
	{
		std::cout << "ERROR: QTranspose*Signals failed" << std::endl;
		return NULL;
	}
	freeDeferred.Add(Qtb);

	// BackSub to get coefficients
	(void)profiler.Lap();
	cl_float* coeffsCalculated = BackSub(R, Qtb, sampleOrder + 1);
	results->AddRunTime(profiler.Lap(), "BackSub Time");
	if (!coeffsCalculated)
	{
		std::cout << "ERROR: BackSub failed to return coefficients" << std::endl;
		return NULL;
	}
	freeDeferred.Add(coeffsCalculated);

	// PolyEval to get Output Signal
	(void)profiler.Lap();
	cl_float* outputSignal = PolyEvalOcl(coeffsCalculated, sampleOrder, outputTimes, sampleOutputCount);
	results->AddRunTime(profiler.Lap(), "PolyEvalOcl Time");
	if (!outputSignal)
	{
		std::cout << "ERROR: PolyEvalOcl failed to return output signal" << std::endl;
		return NULL;
	}

#ifdef VERBOSE_FILE_SAVE
	tools::SaveDataFile("\n\QTranspose:\n", "..\\data\\verbose_output.txt", false);
	tools::SaveDataFile(QTranspose, sampleInputCount, sampleInputCount, "..\\data\\verbose_output.txt", true);
	tools::SaveDataFile("\n\R:\n", "..\\data\\verbose_output.txt", true);
	tools::SaveDataFile(R, sampleInputCount, sampleOrder+1, "..\\data\\verbose_output.txt", true);
	tools::SaveDataFile("\n\nQtb:\n", "..\\data\\verbose_output.txt", true);
	tools::SaveDataFile(Qtb, sampleOutputCount, 1, "..\\data\\verbose_output.txt", true);
	tools::SaveDataFile("\n\nCoeffs:\n", "..\\data\\verbose_output.txt", true);
	tools::SaveDataFile(coeffsCalculated, sampleOrder+1, 1, "..\\data\\verbose_output.txt", true);
#endif

	return outputSignal;
}

// Run Resample Function 
int Run_ResampleOcl(ResultsStruct* results)
{
	ProfilerStruct fullProfiler;
	fullProfiler.Start();
	tools::FreeMemoryFinalizer freeDeferred;

	// Get Settings
	const size_t sampleInputRate = settings::GetTestSampleInputRate();
	const size_t sampleOutputRate = settings::GetTestSampleOutputRate();
	const size_t sampleOrder = settings::GetTestPolynomialOrder();
	const std::string signalDataFile = settings::GetSignalTestDataPath();

	// Read Data File
	size_t signalRows = 0;
	size_t signalCols = 0;
	cl_float* signalInput = tools::LoadDataFile(signalDataFile, &signalRows, &signalCols);
	if (!signalInput || !signalRows || !signalCols)
	{
		std::cout << "ERROR: Failed to load signal data from file: " << signalDataFile << std::endl;
		return -1;
	}
	freeDeferred.Add(signalInput);

	const size_t sampleCount = signalRows;

	// Get QTranspose and R
	cl_float* QTranspose = NULL;
	cl_float* R = NULL;
	cl_float* OutputTimes = NULL;
	cl_float* InputTimes = NULL;
	size_t outputSampleCount = 0;
	bool retVal = Resample_ToQR(sampleInputRate, sampleOutputRate, sampleOrder, sampleCount, &QTranspose, &R, &OutputTimes, &outputSampleCount, &InputTimes);
	if (!retVal)
	{
		std::cout << "ERROR: Failed to build QTranspose and R Matrixes" << std::endl;
		return -1;
	}
	freeDeferred.Add(QTranspose);
	freeDeferred.Add(R);
	freeDeferred.Add(OutputTimes);

	//////////////////////////
	// BEGIN REPEATING PORTION
	// Wrap the repeatable portion in the profiler
	ProfilerStruct profiler;
	profiler.Start();
	cl_float* signalOut = ResampleOcl(signalInput, QTranspose, R, sampleCount, outputSampleCount, sampleOrder, OutputTimes, results);
	profiler.Stop();
	results->AddRunTime(profiler.Log(), "Total Repeating Algorithm Time");

	/////////////////////////
	// Write Out Data
	tools::SaveDataFile(signalInput, sampleCount, 1, settings::GetResampleOuputFile_SignalIn(), false);
	tools::SaveDataFile(InputTimes, sampleCount, 1, settings::GetResampleOuputFile_TimeIn(), false);
	tools::SaveDataFile(signalOut, outputSampleCount, 1, settings::GetResampleOuputFile_SignalOut(), false);
	tools::SaveDataFile(OutputTimes, outputSampleCount, 1, settings::GetResampleOuputFile_TimeOut(), false);

#ifdef VERBOSE_FILE_SAVE
	const std::string verboseFile("..\\data\\verbose_output_end.txt");
	tools::SaveDataFile("\n\nSigIn:\n", verboseFile, false);
	tools::SaveDataFile(signalInput, sampleCount, 1, verboseFile, true);
	tools::SaveDataFile("\n\nTimeIn:\n", verboseFile, true);
	tools::SaveDataFile(InputTimes, sampleCount, 1, verboseFile, true);
	tools::SaveDataFile("\n\nSigOut:\n", verboseFile, true);
	tools::SaveDataFile(signalOut, outputSampleCount, 1, verboseFile, true);
	tools::SaveDataFile("\n\nTimeOut:\n", verboseFile, true);
	tools::SaveDataFile(OutputTimes, outputSampleCount, 1, verboseFile, true);
#endif

	fullProfiler.Stop();
	results->HasWindowsRunTime = true;
	results->WindowsRunTime = fullProfiler.Log();
	return 0;
}


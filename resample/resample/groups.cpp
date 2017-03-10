#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "tools.h"
#include "utils.h"
#include "groups.h"
#include "settings.h"

ResultsStruct::ResultsStruct()
	: WindowsRunTime(0.0)
	, OpenCLRunTime(0.0)
	, HasWindowsRunTime(false)
	, HasOpenCLRunTime(false)
{
	WorkGroupSize[0] = 0;
	WorkGroupSize[1] = 0;
	WorkGroupSize[2] = 0;
}

bool resultTimeOCL(ResultsStruct* A, ResultsStruct* B)
{
	return A->OpenCLRunTime < B->OpenCLRunTime;
}
// Ensure memory is cleared
ResultsList::~ResultsList()
{
	while (!empty())
	{
		ResultsStruct* s = this->back();
		this->pop_back();
		delete s;
	}
}

void PrintToFile(const ResultsList& results)
{
	std::ofstream outfile;
	outfile.open(RESULTS_FILE, std::ios_base::app);
	if (results.empty())
	{
		outfile << "No results";
		return;
	}

	double totalWindowsTimes = 0.0;
	double totalOpenCLTimes = 0.0;
	int num = 0;
	outfile << results.front()->Annotation << std::endl;
	outfile << "Run#, WorkGroupSize, WindowsTime, OpenCLTime" << std::endl;
	for (ResultsList::const_iterator i = results.begin(), e = results.end(); i != e; ++i, ++num)
	{
		totalWindowsTimes += (*i)->WindowsRunTime;
		totalOpenCLTimes += (*i)->OpenCLRunTime;
		outfile << num + 1 << "," << (*i)->WorkGroupSize << "," << (*i)->WindowsRunTime << "," << (*i)->OpenCLRunTime << std::endl;
	}
	const double WindowsAvg = totalWindowsTimes / (double)num;
	const double OpenCLAvg = totalOpenCLTimes / (double)num;

	outfile << "Windows Average: " << WindowsAvg << "; OpenCL Average: " << OpenCLAvg << std::endl;
}

void PrintResults(const ResultsList& results)
{
	if (results.empty())
		return;

	double totalWindowsTimes = 0.0;
	double totalOpenCLTimes = 0.0;
	int num = 0;
	for (ResultsList::const_iterator i = results.begin(), e = results.end(); i != e; ++i, ++num)
	{
		totalWindowsTimes += (*i)->WindowsRunTime;
		totalOpenCLTimes += (*i)->OpenCLRunTime;
		printf("Run: %d: Windows Profiler Runtime: %f ms. OpenCL Profiler Runtime: %f ms.\n", num + 1, (*i)->WindowsRunTime, (*i)->OpenCLRunTime);
	}

	const double WindowsAvg = totalWindowsTimes / (double)num;
	const double OpenCLAvg = totalOpenCLTimes / (double)num;
	printf("---------------------------\n");
	if (results.front()->HasWindowsRunTime)
		printf("Average Windows Profiler Runtime: %f ms.\n", WindowsAvg);
	if (results.front()->HasOpenCLRunTime)
		printf("Average OpenCL Profiler Runtime : %f ms.\n", OpenCLAvg);

	if (PRINT_TO_FILE)
		PrintToFile(results);
}

int ProblemGroup::operator()(int problem)
{
	if (problems_.find(problem) == problems_.end())
	{
		std::cout << "WARNING: " << problem_group_num_ << "." << problem << " not found" << std::endl;
		return 0;
	}

	ResultsList results;
	int retVal = 0;
	const size_t runCount = (GroupNum() == 0 ? 1 : RUN_COUNT);
	for (int i = 0; i < runCount; i++)
	{
		if (GroupNum() != 0)
			printf("\r Running... %d. ", i);
		ResultsStruct* result = new ResultsStruct();
		retVal = problems_[problem]->operator()(result);
		result->Annotation = problems_[problem]->Annotation();
		results.push_back(result);
	}
	if (GroupNum() != 0)
	{
		printf("\n");
		PrintResults(results);
	}

	return retVal;
}
GroupManager::GroupManager(const std::string& name)
	: group_name_(name)
{
}

GroupManager::~GroupManager()
{
	for (std::map<int, ProblemGroup*>::iterator i = groups_.begin(), e = groups_.end(); i != e; ++i)
		delete i->second;
}

void GroupManager::PrintGroupMenu()
{
	std::cout << "Group: " << group_name_.c_str() << std::endl;
	for (std::map<int, ProblemGroup*>::const_iterator i = groups_.begin(), e = groups_.end(); i != e; ++i)
	{
		for (std::map<size_t, Problem*>::const_iterator g = i->second->problems_.begin(), h = i->second->problems_.end(); h != g; ++g)
		{
			std::cout << i->first << "." << g->first << ": " << g->second->Annotation().c_str() << std::endl;
		}
	}
}

int GroupManager::Run()
{
	int result = 0;
	std::cout << "Running new " << group_name_.c_str() << " Tests" << std::endl;
	PrintGroupMenu();
	std::string input;
	do
	{
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Enter #.# to run (-1 to quit): " << std::endl;
		std::cout << "(enter \"?\" for list of functions)" << std::endl;

		std::cin >> input;
		if (input == "?")
		{
			PrintGroupMenu();
			continue;
		}
		if (atoi(input.c_str()) == -1)
			return 0;

		std::vector<std::string> selection = (tools::split(input, "."));
		int problemgroup = atoi(selection[0].c_str());
		int problem = atoi(selection[1].c_str());
		std::cout << "-------------------------------------------------------" << std::endl;
		if (groups_.find(problemgroup) == groups_.end())
		{
			std::cout << "WARNING: " << GroupName().c_str() << " " << problemgroup << " not found." << std::endl;
			continue;
		}
		std::cout << "Running " << GroupName() << " " << problemgroup << "." << problem
				  << ": " << groups_[problemgroup]->problems_[problem]->Annotation() << std::endl;
		result = groups_[problemgroup]->operator()(problem);
		if (result != 0)
			LogInfo("Possible issue with result %d.\n", result);

	} while (atoi(input.c_str()) != -1);
	return result;
}

/////////// Input Gathering /////////////
ProblemGroup* GroupManagerInputControlFactory()
{
	int num = 0;
	ProblemGroup* InputControl = new ProblemGroup(0, "Input Control");
	InputControl->problems_[++num] = new Problem(&SetSampleSize, "Set Signal Sample Size", &GetSampleSizeValueStr);
	InputControl->problems_[++num] = new Problem(&ComparisonThreshold, "Set Maximum Difference for Float Verifications", &GetComparisonThresholdValueStr);
	InputControl->problems_[++num] = new Problem(&RunCount, "Set Number of Runs", &GetRunCountValueStr);
	InputControl->problems_[++num] = new Problem(&PrintResultsToFile, "Set to Print to File", &GetPrintResultsToFileValueStr);
	InputControl->problems_[++num] = new Problem(&SetResultsFile, "Set File Path for Saving Results", &GetResultsFileValueStr);
	return InputControl;
}

std::string GetSampleSizeValueStr()
{
	return std::to_string(SAMPLE_SIZE);
}
int SetSampleSize(ResultsStruct* results)
{
	std::cout << "Enter new sample size: ";
	std::cin >> SAMPLE_SIZE;
	return 0;
}

std::string GetComparisonThresholdValueStr()
{
	return std::to_string(MAX_DIFF);
}
int ComparisonThreshold(ResultsStruct* results)
{
	std::cout << "Enter maximum difference value: ";
	float i = MAX_DIFF;
	std::cin >> i;
	MAX_DIFF = i;
	return 0;
}

std::string GetRunCountValueStr()
{
	return std::to_string(RUN_COUNT);
}
int RunCount(ResultsStruct* results)
{
	std::cout << "Enter number of runs to do: ";
	unsigned int i = RUN_COUNT;
	std::cin >> i;
	RUN_COUNT = i;
	return 0;
}

std::string GetPrintResultsToFileValueStr()
{
	return std::to_string(PRINT_TO_FILE);
}
int PrintResultsToFile(ResultsStruct* results)
{
	std::cout << "Enter 1 to print results to file: ";
	unsigned int i = (unsigned int)PRINT_TO_FILE;
	std::cin >> i;
	PRINT_TO_FILE = (i == 1);
	return 0;
}

std::string GetResultsFileValueStr()
{
	return RESULTS_FILE;
}
int SetResultsFile(ResultsStruct* results)
{
	std::cout << "Enter path to output file to: ";
	char buffer[255];
	std::cin >> buffer;
	buffer[strcspn(buffer, "\n")] = 0;
	RESULTS_FILE = std::string(buffer);
	return 0;
}

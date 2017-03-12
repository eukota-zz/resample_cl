#pragma once
#include <map>
#include <vector>

struct ResultsStruct
{
	ResultsStruct();
	double WindowsRunTime;
	bool HasWindowsRunTime;
	std::string Annotation;
	size_t WorkGroupSize[3];

	bool HasRunTimeVect() {	return !RunTimeVect.empty(); }
	
	void AddRunTime(double runTime, const std::string& desc)
	{
		RunTimeVect.push_back(runTime);
		RunVectDesc.push_back(desc);
	}
	std::vector<double> RunTimeVect;
	std::vector<std::string> RunVectDesc;
};

class ResultsList : public std::vector<ResultsStruct*>
{
public:
	~ResultsList();
};

void PrintToFile(const ResultsList& results);
void PrintResults(const ResultsList& results);

////////////// PROBLEMS
class Problem
{
public:
	Problem(int(*prob)(ResultsStruct*), const std::string& annotation, std::string(*currValFunc)() = NULL)
		: problem_(prob)
		, problem_annotation_(annotation)
		, current_value_(currValFunc)
	{}

	virtual int operator()(ResultsStruct* res)
	{
		return problem_(res);
	};

	std::string Annotation()
	{
		return problem_annotation_ + (current_value_ ? " (current value: " + current_value_() + ")" : std::string());
	}
private:
	std::string problem_annotation_;
	int(*problem_)(ResultsStruct*);
	std::string(*current_value_)(); 
};

////////////// Groups
class ProblemGroup
{
public:
	ProblemGroup(int groupNum, const std::string& annotation) : problem_group_num_(groupNum), problem_group_annotation_(annotation) {}
	int operator()(int problem);

	std::map<size_t, Problem*> problems_;
	int GroupNum() { return problem_group_num_; }

	std::string Annotation() { return problem_group_annotation_; }
protected:
	std::string problem_group_annotation_;
	int problem_group_num_;
};


////////////////// MANAGER
class GroupManager
{
public:
	GroupManager(const std::string& name);
	~GroupManager();
	void PrintGroupMenu();
	int Run();
	std::string GroupName() { return group_name_; }

protected:
	std::string group_name_;
	std::map<int, ProblemGroup*> groups_;
};

ProblemGroup* GroupManagerInputControlFactory();

// SAMPLE_SIZE
std::string GetSampleSizeValueStr();
int SetSampleSize(ResultsStruct* results);

// MAX_DIFF
std::string GetComparisonThresholdValueStr();
int ComparisonThreshold(ResultsStruct* results);

// RUN_COUNT
std::string GetRunCountValueStr();
int RunCount(ResultsStruct* results);

// PRINT_TO_FILE
std::string GetPrintResultsToFileValueStr();
int PrintResultsToFile(ResultsStruct* results);

// RESULTS_FILE
std::string GetResultsFileValueStr();
int SetResultsFile(ResultsStruct* results);

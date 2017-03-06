#pragma once
#include <map>
#include <vector>

struct ResultsStruct
{
	ResultsStruct();
	double WindowsRunTime;
	double OpenCLRunTime;
	bool HasWindowsRunTime;
	bool HasOpenCLRunTime;
	std::string Annotation;
	size_t WorkGroupSize[3];
};

// For use when sorting results lists
bool resultTimeOCL(ResultsStruct* A, ResultsStruct* B);

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
	Problem(int(*prob)(ResultsStruct*), const std::string& annotation) : problem_(prob), problem_annotation_(annotation) {}
	virtual int operator()(ResultsStruct* res)
	{
		return problem_(res);
	};

	std::string Annotation() { return problem_annotation_; }
private:
	std::string problem_annotation_;
	int(*problem_)(ResultsStruct*);
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
	virtual std::string ProblemGroupName() { return std::string(); }
	virtual std::string ProblemName() { return std::string(); }

protected:
	std::string GroupName;
	std::map<int, ProblemGroup*> groups_;
};

ProblemGroup* GroupManagerInputControlFactory();
int SetSampleSize(ResultsStruct* results);
int ComparisonThreshold(ResultsStruct* results);
int SkipVerify(ResultsStruct* results);
int RunCount(ResultsStruct* results);
int PrintResultsToFile(ResultsStruct* results);
int SetResultsFile(ResultsStruct* results);

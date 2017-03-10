#pragma once
#include "groups.h"


class TestGroup: public GroupManager
{
public:
	TestGroup();
	~TestGroup();

	std::map<int, ProblemGroup*> GroupFactory();
};
extern TestGroup* TestGroupObject;




#pragma once
#include "groups.h"


class ResampleGroup : public GroupManager
{
public:
	ResampleGroup();
	~ResampleGroup();

	std::map<int, ProblemGroup*> GroupFactory();
};
extern ResampleGroup* ResampleGroupObject;



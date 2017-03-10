
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <memory.h>
#include <vector>
#include <map>
#include <string>

#include "CL\cl.h"
#include "utils.h"
#include "profiler.h"
#include "tools.h"
#include "ocl.h"
#include "enums.h"
#include "groups.h"
#include "test_group.h"
#include "resample_group.h"
#include "settings.h"

//for perf. counters
#include <Windows.h>

using namespace std;

void PrintInstructions()
{
	cout << endl << "MAIN MENU:" << endl
		<< "// R --> Resample   //" << endl
		<< "// T --> Test       //" << endl
		<< "// Q --> Quit       //" << endl
		<< endl;
}

int _tmain(int argc, TCHAR* argv[])
{
	settings::ReadResamplePrefs();
	(void)settings::GetSignalTestDataPath();

	srand(12345);
	string input;
	do
	{
		int res = 0;
		PrintInstructions();
		cin >> input;
		if (input == "R" || input == "R")
		{
			ResampleGroupObject = new ResampleGroup();
			res = ResampleGroupObject->Run();
			delete ResampleGroupObject;
		}
		if (input == "T" || input == "t")
		{
			TestGroupObject = new TestGroup();
			res = TestGroupObject->Run();
			delete TestGroupObject;
		}
		if (input == "Q" || input == "q")
		{
			break;
		}
		cout << "Results (0 = success): \n" << res << endl;
	} while (true);

	settings::WriteResamplePrefs();
	return 0;
}



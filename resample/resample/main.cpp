
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
#include "control.h"
#include "constants.h"

//for perf. counters
#include <Windows.h>

using namespace std;

void PrintInstructions()
{
	cout << endl << "MAIN MENU:" << endl
		<< "// P --> Project    //" << endl
		<< "// Q --> Quit       //" << endl
		<< endl;
}

int _tmain(int argc, TCHAR* argv[])
{
	prefs::ReadResamplePrefs();
	(void)prefs::GetSignalTestDataPath();

	srand(12345);
	string input;
	do
	{
		int res = 0;
		PrintInstructions();
		cin >> input;
		if (input == "P")
		{
			ControlObject = new ControlClass();
			res = ControlObject->Run();
			delete ControlObject;
		}
		if (input == "Q" || input == "q")
		{
			break;
		}
		cout << "Results (0 = success): \n" << res << endl;
	} while (true);

	prefs::WriteResamplePrefs();
	return 0;
}



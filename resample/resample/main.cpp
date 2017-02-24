
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
	srand(12345);
	bool runTests = false;
	string input;
	do
	{
		int res = 0;
		PrintInstructions();
		cin >> input;
		if (input == "P")
		{
			ControlClass resampleControl;
			res = resampleControl.Run();
		}
		if (input == "Q" || input == "q")
		{
			break;
		}
		cout << "Results (0 = success): \n" << res << endl;
	} while (true);

	return 0;
}



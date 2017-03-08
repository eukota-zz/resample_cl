#include "constants.h"
#include "Shlwapi.h"
#include <fstream>
#include <string>

namespace
{
	const char* PREF_APP = "Resample";
	const char* PREF_FILE = "resample.ini";
	
	const char* PREF_SAMPLE_SIZE = "SampleSize";
	const char* PREF_SKIP_VERIFICATION = "SkipVerification";
	const char* PREF_PRINT_TO_FILE = "PrintToFile";
	const char* PREF_RESULTS_FILE = "ResultsFile";
	const char* PREF_RUN_COUNT = "RunCount";
	const char* PREF_MAX_DIFF = "MaxDiff";
	const char* PREF_PI = "Pi";
}

int SAMPLE_SIZE = 1024;
bool SKIP_VERIFICATION = false;
bool PRINT_TO_FILE = false;
std::string RESULTS_FILE = "results.txt";
unsigned int RUN_COUNT = 1;
float MAX_DIFF = 0.001f;
const float PI = 3.14159265358979323846f;

namespace prefs
{
	std::string GetStringFromPrefs(const std::string& key, const std::string& defVal)
	{
		char buf[255];
		GetPrivateProfileStringA(PREF_APP, key.c_str(), defVal.c_str(), buf, 255, PrefFilePath().c_str());
		return std::string(buf);
	}
	int GetIntFromPrefs(const std::string& key, int defVal)
	{
		return GetPrivateProfileIntA(PREF_APP, key.c_str(), defVal, PrefFilePath().c_str());
	}
	bool GetBoolFromPrefs(const std::string& key, bool defVal)
	{
		return (bool)GetIntFromPrefs(key, defVal ? 1 : 0);
	}
	float GetFloatFromPrefs(const std::string& key, float defVal)
	{
		const std::string defValString = std::to_string(defVal);
		const std::string str = GetStringFromPrefs(key, defValString);
		return std::atof(str.c_str());
	}

	std::string PrefFilePath()
	{
		char buf[255];
		GetFullPathNameA(PREF_FILE, 255, buf, NULL);
		return std::string(buf);
	}

	bool PrefFileExists()
	{
		if (!PathFileExistsA(PrefFilePath().c_str()))
			return false;

		return true;
	}

	bool CreatePrefFile()
	{
		std::ofstream prefsFile;
		prefsFile.open(PrefFilePath(), std::ofstream::out);
		if (!prefsFile.is_open())
			return false;
		
		WritePrefs();
		return true;
	}

	void WritePrefs()
	{
		char pref_file[255];
		GetFullPathNameA(PREF_FILE, 255, pref_file, NULL);
		std::string output;

		output = std::to_string(SAMPLE_SIZE);
		WritePrivateProfileStringA(PREF_APP, PREF_SAMPLE_SIZE, output.c_str(), pref_file);

		output = (SKIP_VERIFICATION ? "1" : "0");
		WritePrivateProfileStringA(PREF_APP, PREF_SKIP_VERIFICATION, output.c_str(), pref_file);

		output = (PRINT_TO_FILE ? "1" : "0");
		WritePrivateProfileStringA(PREF_APP, PREF_PRINT_TO_FILE, output.c_str(), pref_file);

		WritePrivateProfileStringA(PREF_APP, PREF_RESULTS_FILE, RESULTS_FILE.c_str(), pref_file);

		output = std::to_string(RUN_COUNT);
		WritePrivateProfileStringA(PREF_APP, PREF_RUN_COUNT, output.c_str(), pref_file);

		output = std::to_string(MAX_DIFF);
		WritePrivateProfileStringA(PREF_APP, PREF_MAX_DIFF, output.c_str(), pref_file);

		output = std::to_string(PI);
		WritePrivateProfileStringA(PREF_APP, PREF_PI, output.c_str(), pref_file);
	}

	// loads settings from PREF_FILE. If file does not exist, creates file with default values
	void ReadPrefs()
	{
		if (!PrefFileExists())
		{
			(void)CreatePrefFile();
			return;
		}
		SAMPLE_SIZE = GetIntFromPrefs(PREF_SAMPLE_SIZE, SAMPLE_SIZE);
		SKIP_VERIFICATION = GetBoolFromPrefs(PREF_SKIP_VERIFICATION, SKIP_VERIFICATION);
		PRINT_TO_FILE = GetBoolFromPrefs(PREF_PRINT_TO_FILE, PRINT_TO_FILE);
		RESULTS_FILE = GetStringFromPrefs(PREF_RESULTS_FILE, RESULTS_FILE);
		RUN_COUNT = GetIntFromPrefs(PREF_RUN_COUNT, RUN_COUNT);
		MAX_DIFF = GetFloatFromPrefs(PREF_MAX_DIFF, MAX_DIFF);
	}
}
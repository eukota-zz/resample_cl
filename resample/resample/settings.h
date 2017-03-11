#pragma once
#include <string>
// CONSTANTS
extern const float PI;
extern const char* CL_FILENAME;


// SETTINGS
extern int SAMPLE_SIZE;
extern bool PRINT_TO_FILE;
extern std::string RESULTS_FILE;
extern unsigned int RUN_COUNT;
extern float MAX_DIFF;


namespace settings
{
	std::string GetStringFromPrefs(const std::string& prefFile, const std::string& section, const std::string& key, const std::string& defVal);
	int GetIntFromPrefs(const std::string& prefFile, const std::string& section, const std::string& key, int defVal);
	bool GetBoolFromPrefs(const std::string& prefFile, const std::string& section, const std::string& key, bool defVal);
	float GetFloatFromPrefs(const std::string& prefFile, const std::string& section, const std::string& key, float defVal);

	std::string PrefFilePath(const std::string& pref);
	bool PrefFileExists(const std::string& prefFile);
	bool CreatePrefFile(const std::string& prefFile);

	// READ ONCE ON STARTUP
	void WriteResamplePrefs();
	void ReadResamplePrefs();

		// READ ON THE FLY - Can update INI and these will be reread next read
	std::string GetResampleOuputFile_TimeIn();
	std::string GetResampleOuputFile_SignalIn();
	std::string GetResampleOuputFile_TimeOut();
	std::string GetResampleOuputFile_SignalOut();
	void WriteOctavePrefsDefaults();
	std::string GetTestDataPath(const std::string& dataKey);
	std::string GetSignalTestDataPath();
	std::string GetCoeffsTestDataPath();
	std::string GetOutputTestDataPath();
	int GetTestSampleInputRate();
	int GetTestSampleOutputRate();
	int GetTestPolynomialOrder();

}
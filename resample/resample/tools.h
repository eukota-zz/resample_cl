#pragma once
#include <string>
#include <vector>
#include "constants.h"

namespace tools
{
	float GetInput(const std::string& prompt);

	std::vector<std::string> split(const std::string& str, const char* delim);

	// Print an array to std::cout
	// @param[in] a array to print
	// @param[in] s size of array
	// @param[in] asColumn prints a newline between results when true (defaults to false)
	template<class T>
	void printArray(T* a, size_t s, bool asColumn = false)
	{
		if (!a)
			return;

		std::cout << "{ ";
		for (size_t i = 0; i < s; ++i)
		{
			std::cout << a[i];
			if (i + 1 < s)
			{
				std::cout << ", ";
				if (asColumn)
					std::cout << std::endl;
			}
		}
		std::cout << " }";
	};

	// Verify that two arrays are equivalent
	// @param[in] a left input array
	// @param[in] b right input array
	// @param[in] s array size - assumed to match
	// @return true if equivalent, false otherwise
	template<class T>
	bool isEqual(T* a, T* b, size_t s)
	{
		if (!a || !b)
			return false;
		for (size_t i = 0; i < s; ++i)
		{
			if (std::abs(a[i] - b[i]) > MAX_DIFF)
				return false;
		}
		return true;
	}

	float* IncrementalArrayGenerator_ByStep(float start, float end, float stepVal);
	float* IncrementalArrayGenerator_BySize(float start, float stepVal, size_t sampleCount);
	void AMatrixGenerator(float* input, size_t numSamples, size_t order, float* aMatrix);
	void SignalGenerator(float sampleRate, float freq, float noiseLevel, size_t numSamples, float* resReal, float* resImag);

}

struct ResultsStruct;
int Test_SignalGenerator(ResultsStruct* results);
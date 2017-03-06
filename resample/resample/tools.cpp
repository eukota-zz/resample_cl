#include <string>
#include <iostream>
#include <vector>

#include "tools.h"

namespace tools
{
	float GetInput(const std::string& prompt)
	{
		std::cout << prompt;
		float v;
		std::cin >> v;
		return v;
	}

	std::vector<std::string> split(const std::string& str, const char* delim)
	{
		std::vector<std::string> v;
		if (str.empty())
			return v;

		size_t begin = 0;
		for (size_t i = 0; i <= str.size(); i++)
		{
			if (str[i] != *delim && i != str.size())
				continue;

			v.push_back(str.substr(begin, i - begin));
			begin = i + 1;
		}
		return v;
	}


	// Fills output with an array of values from start to end (inclusive) stepping by step
	// @param[in] start value to start at
	// @param[in] end value to end at (is included in samples)
	// @param[in] stepVal value to step by
	// @return resulting generated points - Caller is responsible for freeing memory
	float* IncrementalArrayGenerator_ByStep(float start, float end, float stepVal)
	{
		if (stepVal <= 0 || start-end < stepVal)
			return NULL;

		const size_t sampleCount = ((end - start) / stepVal);
		return IncrementalArrayGenerator_BySize(start, stepVal, sampleCount);
	}
	float* IncrementalArrayGenerator_BySize(float start, float stepVal, size_t sampleCount)
	{
		if (stepVal <= 0 || !sampleCount)
			return NULL;

		float* results = (float*)malloc(sizeof(float)*sampleCount);
		for (size_t i = 0; i < sampleCount; ++i)
			results[i] = start + i*stepVal;

		return results;
	}

	// Creates the A Matrix for QR Decomposition - size numSamples by (order+1)
	// @param[in] input array of size numSamples 
	// @param[in] numSamples number of samples to use (height of matrix)
	// @param[in] order polynomial order (width of matrix - 1)
	// @param[out] aMatirx the A matrix of size (numSamples)x(order+1)
	void AMatrixGenerator(float* input, size_t numSamples, size_t order, float* aMatrix)
	{
		if (!input || !numSamples || !order || !aMatrix)
			return;

		const size_t width = order + 1;
		for (size_t i = 0; i < numSamples; ++i)
		{
			for (int j = 0; j < width; ++j)
				aMatrix[i*width + j] = pow(input[i], j);
		}
	}

	// Generates sinusoidal complex IQ signal
	// @param[in] sampleRate the sample rate in samples per second
	// @param[in] freq the frequency of the generated waves
	// @param[in] noiseLevel amplitude of noise (0 is no noise)
	// @param[in] numSamples the number of points to calculate
	// @param[out] resReal the real component results of the generated signal
	// @param[out] resImag the imaginary component results of the generated signal
	void SignalGenerator(float sampleRate, float freq, float noiseLevel, size_t numSamples, float* resReal, float* resImag)
	{
		if (!sampleRate || !resReal || !resImag)
			return;

		const float twoPiF = 2 * PI * freq;
		const float deltaT = 1 / sampleRate;
		for (size_t i = 0; i < numSamples; ++i)
		{
			const float t = i*deltaT;
			resReal[i] = cos(twoPiF * t) + noiseLevel * (2 * (float)rand() / (float)RAND_MAX - 1);
			resImag[i] = sin(twoPiF * t) + noiseLevel * (2 * (float)rand() / (float)RAND_MAX - 1);
		}
	}
}

struct ResultsStruct; // forward declare

// @todo write test...
int Test_IncrementalArrayGenerator(ResultsStruct* results)
{
	// by step
	// invalid - 1:-1:100, 100:1:1, 1:0:100, 1:0.1:1
	// 1:1:100
	// expect: 

	// by size
	// invalid - 1,0,10     1,10,0
	// 1,1,10
	return -1;
}

// Tests SignalGenerator
int Test_SignalGenerator(ResultsStruct* results)
{
	float sampleRate = 10;
	float freq = 1;
	float noise = 1;
	const size_t numSamples = 100;
	float realData[numSamples];
	float imagData[numSamples];
	tools::SignalGenerator(sampleRate, freq, noise, numSamples, realData, imagData);

	// no way to really verify so just print out results
	std::cout << "I Data: " << std::endl;
	tools::printArray<float>(realData, numSamples);
	std::cout << std::endl << "Q Data: " << std::endl;
	tools::printArray<float>(imagData, numSamples);
	std::cout << std::endl;

	return 0;
}


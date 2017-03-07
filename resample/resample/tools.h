#pragma once
#include <string>
#include <vector>
#include <iomanip>
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

	// Print a matrix to std::cout
	// @param[in] M matrix to print
	// @param[in] rows number of rows (height) 
	// @param[in] cols number of cols (width)
	template<class T>
	void printMatrix(T* M, size_t rows, size_t cols)
	{
		if (!M)
			return;

		std::cout << std::showpos << std::fixed << std::setprecision(5);
		for (size_t i = 0; i < rows; ++i)
		{
			std::cout << "[ ";
			for (size_t j = 0; j < cols; ++j)
			{
				std::cout << M[i*cols + j];
				if (j + 1 < cols)
					std::cout << ", ";
			}
			std::cout << " ]" << std::endl;
		}
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

	float* CreateIdentityMatrix(size_t size);
	void TransposeMatrix(float* input, size_t rows, size_t cols, float* output);
	float* MatrixMultiplier(float* matrixA, size_t rowsA, size_t colsA, float* matrixB, size_t rowsB, size_t colsB);
	float* IncrementalArrayGenerator_ByStep(float start, float end, float stepVal);
	float* IncrementalArrayGenerator_BySize(float start, float stepVal, size_t sampleCount);
	float* AMatrixGenerator(float* input, size_t numSamples, size_t order);
	void SignalGenerator(float sampleRate, float freq, float noiseLevel, size_t numSamples, float* resReal, float* resImag);
}

struct ResultsStruct;
int Test_CreateIdentityMatrix(ResultsStruct* results);
int Test_SignalGenerator(ResultsStruct* results);
int Test_AMatrixGenerator(ResultsStruct* results);
int Test_MatrixMultiplier(ResultsStruct* results);
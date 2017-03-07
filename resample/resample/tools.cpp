#include <string>
#include <iostream>
#include <vector>
#include "tools.h"

#include "utils.h"

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

	// Transposes the input matrix
	// @param[in] input matrix to transpose
	// @param[in] rows height of input matrix
	// @param[in] cols width of input matrix
	// @param[out] output transposed matrix
	void TransposeMatrix(float* input, size_t rows, size_t cols, float* output)
	{
		if (!input || !rows || !cols || !output)
			return;

		for (size_t i = 0; i < rows; ++i)
		{
			for (size_t j = 0; j < cols; ++j)
				output[j*rows + i] = input[i*cols + j];
		}
	}

	// Multiplies two matrices together [A]*[B] where A is MxN and B is NxL
	// @param[in] matrixA left matrix
	// @param[in] rowsA number of rows of A (M)
	// @param[in] colsA number of columns of A (N)
	// @param[in] matrixB right matrix
	// @param[in] rowsB number of rows ob B (N - must match colsA)
	// @param[in] colsB number of columns of B (L)
	// @return output result
	// NOTE: CALLER TAKES OWNERSHIP OF RETURN VALUE
	float* MatrixMultiplier(float* matrixA, size_t rowsA, size_t colsA, float* matrixB, size_t rowsB, size_t colsB)
	{
		if (!matrixA || !rowsA || !colsA || !matrixB || !rowsB || !colsB)
		{
			LogError("MatrixMultiplier called with invalid data.");
			return NULL;
		}
		if (colsA != rowsB)
		{
			LogError("MatrixMultiplier called with unmatching sizes.");
			return NULL;
		}

		const size_t height = rowsA;
		const size_t width = colsB;
		const size_t products = colsA; // number of products for each output cell
		float* output = (float*)malloc(sizeof(float)*width*height);
		for (size_t row = 0; row < height; ++row)
		{
			for (size_t col = 0; col < width; ++col)
			{
				const size_t idx = row*width + col;
				output[idx] = 0.0f;
				for (size_t innerIdx = 0; innerIdx < products; ++innerIdx)
					output[idx] += matrixA[row*colsA + innerIdx] * matrixB[innerIdx*colsB + col];
			}
		}
		return output;
	}

	// Creates Identity matrix of size size in output
	// @param[in] size size of the identity matrix
	// @param[out] output identity matrix output
	void CreateIdentityMatrix(size_t size, float* output)
	{
		if (!output)
			return;

		for (size_t i = 0; i < size; ++i)
		{
			for (size_t j = 0; j < size; ++j)
			{
				if (i == j)
					output[i*size + j] = 1.0f;
				else
					output[i*size + j] = 0.0f;
			}
		}
	}

	// Fills output with an array of values from start to end (inclusive) stepping by step
	// @param[in] start value to start at
	// @param[in] end value to end at (is included in samples)
	// @param[in] stepVal value to step by
	// @return resulting generated points - Caller is responsible for freeing memory
	// NOTE: CALLER TAKES OWNERSHIP OF RETURN VALUE
	float* IncrementalArrayGenerator_ByStep(float start, float end, float stepVal)
	{
		if (stepVal <= 0 || start-end < stepVal)
			return NULL;

		const size_t sampleCount = (size_t)((end - start) / stepVal);
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
	// NOTE: CALLER TAKES OWNERSHIP OF RETURN VALUE
	float* AMatrixGenerator(float* input, size_t numSamples, size_t order)
	{
		if (!input || !numSamples || !order)
			return NULL;

		const size_t width = order + 1;
		float* aMatrix = (float*)malloc(sizeof(float)*numSamples*width);

		for (size_t i = 0; i < numSamples; ++i)
		{
			for (int j = 0; j < width; ++j)
				aMatrix[i*width + j] = pow(input[i], j);
		}
		return aMatrix;
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

// Tests AMatrixGenerator
int Test_AMatrixGenerator(ResultsStruct* results)
{
	const size_t numSamples = 3;
	float input[numSamples] = { 1, 2, 3 };
	const size_t order = 2;

	std::cout << "Test Simple Input: ";
	try
	{
		float aMatrixExpected[] = { 1, 1, 1,
									1, 2, 4,
									1, 3, 9 };
		float* aMatrix = tools::AMatrixGenerator(input, numSamples, order);
		for (size_t row = 0; row < numSamples; ++row)
		{
			for (size_t col = 0; col < order + 1; ++col)
			{
				const size_t idx = row*(order + 1) + col;
				if (aMatrixExpected[idx] != aMatrix[idx])
					throw;
			}
		}
		free(aMatrix);
				
	}
	catch (...)
	{
		std::cout << "FAIL - AMatrix calculation did not match" << std::endl;
		return -1;
	}
	std::cout << "SUCCESS" << std::endl;

	std::cout << "Test Invalid Input Array: ";
	try
	{
		float* aMatrix = tools::AMatrixGenerator(NULL, numSamples, order);
		if (aMatrix)
		{
			free(aMatrix);
			throw;
		}
	}
	catch (...)
	{
		std::cout << "FAIL - should not init AMatrix on invalid input array" << std::endl;
		return -1;
	}
	std::cout << "SUCCESS" << std::endl;

	std::cout << "Test Invalid Sample Count: ";
	try
	{
		float* aMatrix = tools::AMatrixGenerator(input, 0, 2);
		if (aMatrix)
		{
			free(aMatrix);
			throw;
		}
	}
	catch (...)
	{
		std::cout << "FAIL - should not init AMatrix on invalid sample count" << std::endl;
		return -1;
	}
	std::cout << "SUCCESS" << std::endl;

	std::cout << "Test Invalid Order: ";
	try
	{
		float* aMatrix = tools::AMatrixGenerator(input, 3, 0);
		if (aMatrix)
		{
			free(aMatrix);
			throw;
		}
	}
	catch (...)
	{
		std::cout << "FAIL - should not init AMatrix on invalid order" << std::endl;
		return -1;
	}
	std::cout << "SUCCESS" << std::endl;

	return 0;
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

// Tests MatrixMultiplier
int Test_MatrixMultiplier(ResultsStruct* results) 
{
	std::cout << "Test Matrix Multiply: ";
	{
		float matrixA[] = { 1.0f, 2.0f, 3.0f,
							4.0f, 5.0f, 6.0f };
		const size_t rowsA = 2;
		const size_t colsA = 3;
		float matrixB[] = { 1.0f, 2.0f,
							3.0f, 4.0f,
							5.0f, 6.0f };
		const size_t rowsB = 3;
		const size_t colsB = 2;
		float* output = tools::MatrixMultiplier(matrixA, rowsA, colsA, matrixB, rowsB, colsB);

		float expected[] = { 22.00000f, 28.00000f,
							 49.00000f, 64.00000f };

		if (!tools::isEqual<float>(output, expected, rowsA*colsB))
		{
			std::cout << "FAIL" << std::endl;
			std::cout << "Matrix A: " << std::endl;
			tools::printMatrix<float>(matrixA, rowsA, colsA);
			std::cout << std::endl;
			std::cout << "Matrix B: " << std::endl;
			tools::printMatrix<float>(matrixB, rowsB, colsB);
			std::cout << std::endl;

			std::cout << "EXPECTED PRODUCT: " << std::endl;
			tools::printMatrix<float>(expected, rowsA, colsB);
			std::cout << std::endl;
			std::cout << "ACTUAL PRODUCT: " << std::endl;
			tools::printMatrix<float>(output, rowsA, colsB);
			std::cout << std::endl;
		}
		else
			std::cout << "SUCCESS" << std::endl;

		free(output);
	}
	std::cout << "Test Matrix by Column Vector: ";
	{
		float matrixA[] = { 1.0f, 2.0f, 3.0f,
							4.0f, 5.0f, 6.0f,
							7.0f, 8.0f, 9.0f };
		const size_t rowsA = 3;
		const size_t colsA = 3;
		float matrixB[] = { 1.0f,
							2.0f,
							3.0f };
		const size_t rowsB = 3;
		const size_t colsB = 1;
		float* output = tools::MatrixMultiplier(matrixA, rowsA, colsA, matrixB, rowsB, colsB);

		float expected[] = { 14.0f,
							 32.0f,
							 50.0f };

		if (!tools::isEqual<float>(output, expected, rowsA*colsB))
		{
			std::cout << "FAIL" << std::endl;
			std::cout << "Matrix A: " << std::endl;
			tools::printMatrix<float>(matrixA, rowsA, colsA);
			std::cout << std::endl;
			std::cout << "Matrix B: " << std::endl;
			tools::printMatrix<float>(matrixB, rowsB, colsB);
			std::cout << std::endl;

			std::cout << "EXPECTED PRODUCT: " << std::endl;
			tools::printMatrix<float>(expected, rowsA, colsB);
			std::cout << std::endl;
			std::cout << "ACTUAL PRODUCT: " << std::endl;
			tools::printMatrix<float>(output, rowsA, colsB);
			std::cout << std::endl;
		}
		else
			std::cout << "SUCCESS" << std::endl;

		free(output);
	}

	return 0;
}

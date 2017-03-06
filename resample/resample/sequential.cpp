#include "sequential.h"
#include "utils.h"
#include "tools.h"
#include "stdlib.h"
#include "control.h"
#include "profiler.h"
#include "groups.h"
#include "CL/cl.h"

// Calculate Q and R matrixes for QR Decomposition of matrix
void QR(cl_float* R, cl_float* Q, cl_uint arrayWidth, cl_uint arrayHeight)
{
	cl_float a;
	cl_float b;
	cl_float c;
	cl_float s;
	cl_float r;
	cl_float Rnew1[2048];
	cl_float Rnew2[2048];
	cl_float Qnew1[2048];
	cl_float Qnew2[2048];
	for (int j = 0; j < arrayWidth; j++)
	{
		for (int i = arrayHeight - 1; i > j; i--)
		{
			// Calculate Givens rotations
			a = R[arrayWidth * (i - 1) + j];
			b = R[arrayWidth * i + j];
			r = sqrt(a * a + b * b);
			c = a / r;
			s = -b / r;
			// Zero out elements in R matrix
			for (int k = j; k < arrayWidth; k++)
			{
				Rnew1[k] = R[arrayWidth * (i - 1) + k] * c - R[arrayWidth * i + k] * s;
				Rnew2[k] = R[arrayWidth * (i - 1) + k] * s + R[arrayWidth * i + k] * c;
			}
			// Copy new values back to R matrix
			for (int k = j; k < arrayWidth; k++)
			{
				R[arrayWidth * (i - 1) + k] = Rnew1[k];
				R[arrayWidth * i + k] = Rnew2[k];
			}
			// Update Q matrix
			for (int k = 0; k < arrayHeight; k++)
			{
				Qnew1[k] = Q[arrayHeight * (i - 1) + k] * c + Q[arrayHeight * i + k] * s;
				Qnew2[k] = -Q[arrayHeight * (i - 1) + k] * s + Q[arrayHeight * i + k] * c;
			}
			for (int k = 0; k < arrayHeight; k++)
			{
				Q[arrayHeight * (i - 1) + k] = Qnew1[k];
				Q[arrayHeight * i + k] = Qnew2[k];
			}
		}
	}
}

// Tests QR
int Test_QR(ResultsStruct* results)
{
	const cl_uint arrayWidth = 3;
	const cl_uint arrayHeight = 5;
	cl_uint numIter = 10000; // Number of iterations for runtime averaging

							 // allocate working buffers. 
							 // the buffer should be aligned with 4K page and size should fit 64-byte cached line
	cl_uint optimizedSize = ((sizeof(cl_float) * arrayWidth * arrayHeight - 1) / 64 + 1) * 64;
	cl_float* A = (cl_float*)_aligned_malloc(optimizedSize, 4096);

	optimizedSize = ((sizeof(cl_float) * arrayHeight * arrayHeight - 1) / 64 + 1) * 64;
	cl_float* Q = (cl_float*)_aligned_malloc(optimizedSize, 4096);

	cl_float Atmp[] = { 0.8147, 0.0975, 0.1576,
		0.9058, 0.2785, 0.9706,
		0.1270, 0.5469, 0.9572,
		0.9134, 0.9575, 0.4854,
		0.6324, 0.9649, 0.8003 };

	cl_float Qtmp[] = { 1.0,   0,   0,   0,   0,
		0, 1.0,   0,   0,   0,
		0,   0, 1.0,   0,   0,
		0,   0,   0, 1.0,   0,
		0,   0,   0,   0, 1.0 };

	if (NULL == A)
	{
		LogError("Error: _aligned_malloc failed to allocate buffers.\n");
		return -1;
	}

	// Initialize A
	for (int i = 0; i < arrayWidth * arrayHeight; i++)
	{
		A[i] = Atmp[i];
	}

	// Initialize Q
	for (int i = 0; i < arrayHeight * arrayHeight; i++)
	{
		Q[i] = Qtmp[i];
	}

	// add
	ProfilerStruct profiler;
	profiler.Start();

	QR(A, Q, arrayWidth, arrayHeight);

	profiler.Stop();
	float runTime = profiler.Log();

	_aligned_free(A);

	results->WindowsRunTime = (double)runTime;
	results->HasWindowsRunTime = true;
	return 0;
}

// Solves for coefficients in equation R*x=Q*b where
// R and Q are from QR Decomposition
// x is desired coefficients
// b is column vector of sampled data
// @param[in] R upper triangular matrix of size MxN (from QR Decomposition)
// @param[in] Qtb Q*b column vector of size Mx1
// @param[in] Dim coefficients to solve for - should match N columns size of R
// @param[out] Result is where x is stored
void BackSub(cl_float* R, cl_float* Qtb, size_t Dim, cl_float* Result)
{
	if (!R || !Qtb || Dim == 0)
		return;

	// Start at the Nth row in the right-triangular matrix R where we effectively have R[n][n] * x[n] = Qtb[n]
	// Work our way up solving for each x[n] value in reverse, allowing us to solve for each row as we work our way up
	for (size_t i = Dim; i > 0; --i)
	{
		const size_t resultIdx = i - 1;
		const size_t RRowStartIdx = resultIdx*Dim; // since R is indexed as an array instead of a matrix

		// sum up product of remaining row of R with known x values
		cl_float subtractSum = 0;
		for (size_t sumIdx = resultIdx + 1; sumIdx < Dim; sumIdx++)
			subtractSum += R[RRowStartIdx + sumIdx] * Result[sumIdx];

		// calculate final result eg: if ax + b = c, then x = (c-b)/a
		if(Result)
			Result[resultIdx] = (Qtb[resultIdx] - subtractSum) / R[RRowStartIdx + resultIdx];
	}
}

// Tests BackSub
int Test_BackSub(ResultsStruct* results)
{
	std::cout << std::endl;
	std::cout << "Simple Test: ";
	{
		const size_t cols = 3;
		float Qtb[] = {4, -1, 2};
		float R[] = { 1, -2,  1,
					  0,  1,  6,
					  0,  0,  1 };
		float Result[cols];
		float Expected[] = { -24, -13, 2 };

		BackSub(R, Qtb, cols, Result);
		if (tools::isEqual<float>(Result, Expected, cols))
		{
			std::cout << "SUCCESS!" << std::endl;
		}
		else
		{
			std::cout << "FAILURE!" << std::endl;
			std::cout << "Expected: " << std::endl;
			tools::printArray<float>(Expected, cols);
			std::cout << "Actual: " << std::endl;
			tools::printArray<float>(Result, cols);
		}
	}
	std::cout << "Decimals Test: ";
	{
		const size_t cols = 5;
		float Qtb[] = { 0.74882, 0.95545, 0.37916, 0.84571, 0.71664 };
		float R[] = { 0.845668,   0.776442,   0.049804,   0.223160,   0.234288,
					  0.321790,   0.953842,   0.344031,   0.673007,   0.565058,
					  0.041129,   0.496991,   0.221390,   0.170497,   0.844328,
					  0.814968,   0.691170,   0.545236,   0.780255,   0.518466,
					  0.863745,   0.461225,   0.834668,   0.176055,   0.305269 };
		float Result[cols];
		float Expected[] = { -1.46197, 2.4261, -6.87383, -0.47603, 2.34757 };

		BackSub(R, Qtb, cols, Result);
		if (tools::isEqual<float>(Result, Expected, cols))
		{
			std::cout << "SUCCESS!" << std::endl;
		}
		else
		{
			std::cout << "FAILURE!" << std::endl;
			std::cout << "Expected: " << std::endl;
			tools::printArray<float>(Expected, cols);
			std::cout << "Actual: " << std::endl;
			tools::printArray<float>(Result, cols);
		}
	}
	return 0;
}

// Sequentially evaluate polynomial results for input values with coefficients
// @param[in] coeffs array of coefficients of size order+1
// @param[in] order order of the polynomial
// @param[in] input array of input values
// @param[in] numSamples number of input values
// @param[out] output array of calculated values
void PolyEval(cl_float* coeffs, size_t order, cl_float* input, size_t numSamples, cl_float* output)
{
	// for each data point
	for (size_t valIdx = 0; valIdx < numSamples; ++valIdx)
	{
		output[valIdx] = 0;
		// sum: a0*x^0 + a1*x^1 + a2*x^2 + ... + an*x^n
		for (size_t coef = 0; coef <= order; ++coef)
		{
			output[valIdx] += coeffs[coef] * pow(input[valIdx], coef);
		}
	}
}

// Tests PolyEval
int Test_PolyEval(ResultsStruct* results)
{
	std::cout << std::endl;
	std::cout << "Simple Test: ";
	{
		float coeffs[] = { 1, 1, 1 };
		const size_t order = 2;

		float input[] = { 0, 1, 2, 3, 4, 5 };
		const size_t numSamples = 6;

		float Result[numSamples];
		float Expected[] = { 1, 3, 7, 13, 21, 31 };

		PolyEval(coeffs, order, input, numSamples, Result);
		if (tools::isEqual<float>(Result, Expected, numSamples))
		{
			std::cout << "SUCCESS!" << std::endl;
		}
		else
		{
			std::cout << "FAILURE!" << std::endl;
			std::cout << "Expected: " << std::endl;
			tools::printArray<float>(Expected, numSamples);
			std::cout << "Actual: " << std::endl;
			tools::printArray<float>(Result, numSamples);
		}
	}
	std::cout << "Generated Data Test: ";
	{
		float coeffs[] = { 1, 2, 3, 4, 5, 6, 7 };
		const size_t order = 6;

		// @todo get input by using input generation functions
		float input[] = { 0, 1, 2, 3, 4, 5 };
		const size_t numSamples = 6;

		float Result[numSamples];
		float Expected[] = { 1, 1, 1, 1, 1, 1 };

		PolyEval(coeffs, order, input, numSamples, Result);
		if (tools::isEqual<float>(Result, Expected, numSamples))
		{
			std::cout << "SUCCESS!" << std::endl;
		}
		else
		{
			std::cout << "FAILURE!" << std::endl;
			std::cout << "Expected: " << std::endl;
			tools::printArray<float>(Expected, numSamples);
			std::cout << "Actual: " << std::endl;
			tools::printArray<float>(Result, numSamples);
		}
	}
	return 0;
}

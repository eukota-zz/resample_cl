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
	for (size_t j = 0; j < arrayWidth; j++)
	{
		for (size_t i = arrayHeight - 1; i > j; i--)
		{
			// Calculate Givens rotations
			a = R[arrayWidth * (i - 1) + j];
			b = R[arrayWidth * i + j];
			r = sqrt(a * a + b * b);
			c = a / r;
			s = -b / r;
			// Zero out elements in R matrix
			for (size_t k = j; k < arrayWidth; k++)
			{
				Rnew1[k] = R[arrayWidth * (i - 1) + k] * c - R[arrayWidth * i + k] * s;
				Rnew2[k] = R[arrayWidth * (i - 1) + k] * s + R[arrayWidth * i + k] * c;
			}
			// Copy new values back to R matrix
			for (size_t k = j; k < arrayWidth; k++)
			{
				R[arrayWidth * (i - 1) + k] = Rnew1[k];
				R[arrayWidth * i + k] = Rnew2[k];
			}
			// Update Q matrix
			for (size_t k = 0; k < arrayHeight; k++)
			{
				Qnew1[k] = Q[arrayHeight * (i - 1) + k] * c + Q[arrayHeight * i + k] * s;
				Qnew2[k] = -Q[arrayHeight * (i - 1) + k] * s + Q[arrayHeight * i + k] * c;
			}
			for (size_t k = 0; k < arrayHeight; k++)
			{
				Q[arrayHeight * (i - 1) + k] = Qnew1[k];
				Q[arrayHeight * i + k] = Qnew2[k];
			}
		}
	}
}

// Tests QR Decomposition
// Will print out Expected and Actual Q and/or R if it fails
int Test_QR(ResultsStruct* results)
{
	std::cout << "Tes QR Decomposition: ";
	const size_t arrayWidth = 3;
	const size_t arrayHeight = 5;

	float* A = (float*)malloc(sizeof(float)*arrayWidth*arrayHeight);
	float* Q = (float*)malloc(sizeof(float)*arrayHeight*arrayHeight);


	float Atmp[] = { 1.000000, 0.000000,  0.000000,
					 1.000000, 1.000000,  1.000000,
					 1.000000, 2.000000,  4.000000,
					 1.000000, 3.000000,  9.000000,
					 1.000000, 4.000000, 16.000000 };

	// Initialize A
	for (size_t i = 0; i < arrayWidth * arrayHeight; ++i)
		A[i] = Atmp[i];

	// Initialize Q
	tools::CreateIdentityMatrix(arrayHeight, Q);

	// Perform QR Decomposition
	ProfilerStruct profiler;
	profiler.Start();
	QR(A, Q, arrayWidth, arrayHeight);
	profiler.Stop();
	float runTime = profiler.Log();

	float RExpected[] = { 2.23607f,  4.47214f, 13.41641f,
						  0.00000f,  3.16228f,  12.64911f,
						  0.00000f,  0.00000f,  3.74166f,
						  0.00000f,  0.00000f,  0.00000f,
						  0.00000f,  0.00000f,  0.00000f};

	float QExpected[] = { 0.44721f,  0.63246f,  0.53452f,  0.33806f,  0.00000f,
						 -0.44721f, -0.31623f,  0.26726f,  0.76064f,  0.22361f,
						  0.44721f, -0.00000f, -0.53452f,  0.25355f,  0.67082f,
						 -0.44721f,  0.31623f,  0.26726f, -0.42258f,  0.67082f,
						  0.44721f, -0.63246f,  0.53452f, -0.25355f,  0.22361f};
	bool failed = false;
	if (!tools::isEqual<float>(A, RExpected, arrayWidth*arrayHeight))
	{
		std::cout << "FAIL" << std::endl;

		std::cout << "A: " << std::endl;
		tools::printMatrix<float>(Atmp, arrayHeight, arrayWidth);
		std::cout << "EXPECTED R: " << std::endl;
		tools::printMatrix<float>(A, arrayHeight, arrayWidth);
		std::cout << "ACTUAL R: " << std::endl;
		tools::printMatrix<float>(RExpected, arrayHeight, arrayWidth);

		failed = true;
	}
	float* QTranspose = (float*)malloc(sizeof(float)*arrayHeight*arrayHeight);
	tools::TransposeMatrix(Q, arrayHeight, arrayHeight, QTranspose);
	if(!tools::isEqual<float>(QTranspose, QExpected, arrayHeight*arrayHeight))
	{
		std::cout << "FAIL" << std::endl;

		std::cout << "A: " << std::endl;
		tools::printMatrix<float>(Atmp, arrayHeight, arrayWidth);
		std::cout << "EXPECTED Q: " << std::endl;
		tools::printMatrix<float>(QExpected, arrayHeight, arrayHeight);
		std::cout << "ACTUAL Q: " << std::endl;
		tools::printMatrix<float>(QTranspose, arrayHeight, arrayHeight);

		failed = true;
	}
	if(!failed)
		std::cout << "SUCCESS" << std::endl;

	free(QTranspose);
	free(A);
	free(Q);

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
		float Qtb[] = { 0.74882f, 0.95545f, 0.37916f, 0.84571f, 0.71664f };
		float R[] = { 0.845668f,   0.776442f,   0.049804f,   0.223160f,   0.234288f,
					  0.321790f,   0.953842f,   0.344031f,   0.673007f,   0.565058f,
					  0.041129f,   0.496991f,   0.221390f,   0.170497f,   0.844328f,
					  0.814968f,   0.691170f,   0.545236f,   0.780255f,   0.518466f,
					  0.863745f,   0.461225f,   0.834668f,   0.176055f,   0.305269f };
		float Result[cols];
		float Expected[] = { -1.46197f, 2.4261f, -6.87383f, -0.47603f, 2.34757f };

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
			output[valIdx] += coeffs[coef] * (float)pow(input[valIdx], coef);
	}
}

// Tests PolyEval
int Test_PolyEval(ResultsStruct* results)
{
	std::cout << std::endl;
	std::cout << "Simple Test 1: ";
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
	std::cout << std::endl << "Simple Test 2: ";
	{
		float coeffs[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
		const size_t order = 7;

		float input[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
		const size_t numSamples = 10;

		float Result[numSamples];
		float Expected[] = { 36, 1793, 24604, 167481, 756836, 2620201, 7526268, 18831568, 42374116, 87654320 };
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

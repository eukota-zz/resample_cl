#include "sequential.h"
#include "utils.h"
#include "tools.h"
#include "stdlib.h"
#include "control.h"
#include "profiler.h"
#include "groups.h"
#include "CL/cl.h"

// Resample data from inputFile assuming inputRate and resampling at outputRate using LSA polynomial of order order
// Writes results to directory named "inputFile" to be used for plotting
// @param[in] inputFile path to signal data - expected to be sampleSize-by-1
// @param[in] inputRate input sample rate in samples per second
// @param[in] outputRate output sample rate in samples per second
// @param[in] order polynomial order to use
// @param[out] coeffs coefficients of LSA polynomial
// @return resampled data
cl_float* Resample(const std::string& inputFile, size_t inputRate, size_t outputRate, size_t order, cl_float** coeffs, bool verbose)
{
	// Load Signal Data
	size_t rows = 0;
	size_t cols = 0;
	float* signalData = tools::LoadDataFile(inputFile, &rows, &cols);
	if (verbose)
		tools::SaveDataFile(signalData, rows, cols, "..\\data\\ResampleTest_signalIn.csv", false);

	if (cols > 1)
	{
		LogError("Resample signal data file has too many inputs");
		free(signalData);
		return NULL;
	}
	const size_t sampleCount = rows;

	// create input time vector
	const float inputTimeStep = 1 / (float)inputRate;
	const float inputTimeEnd = (float)sampleCount / (float)inputRate;
	float* inputTimes = tools::IncrementalArrayGenerator_ByStep(inputTimeStep, inputTimeEnd, inputTimeStep);
	if (verbose)
		tools::SaveDataFile(inputTimes, sampleCount, 1, "..\\data\\ResampleTest_timeIn.csv", false);

	// create A matrix
	float* matrixA = tools::GenerateAMatrix(inputTimes, sampleCount, order);

	// perform QR to get Q and R matrixes
	cl_float* R = tools::CopyMatrix(matrixA, sampleCount, order + 1);
	cl_float* QTranspose = tools::CreateIdentityMatrix(sampleCount);
	QR(R, QTranspose, order+1, sampleCount); // outputs QTranspose

	// multiply Q by signal input data to get Qtb
	cl_float* Qtb = tools::MatrixMultiplier(QTranspose, sampleCount, sampleCount, signalData, sampleCount, 1);

	// perform BackSub to get coefficients
	cl_float* coeffsCalculated = BackSub(R, Qtb, order+1);

	// Generate output sample times to evaulate at
	const float outputTimeStep = 1.0f / (float)outputRate;
	const float outputTimeEnd = inputTimeEnd;
	const size_t outputSampleSize = (size_t)(outputTimeEnd * outputRate);
	cl_float* outputTimeValues = tools::IncrementalArrayGenerator_ByStep(outputTimeStep, outputTimeEnd, outputTimeStep);
	if (verbose)
		tools::SaveDataFile(outputTimeValues, outputSampleSize, 1, "..\\data\\ResampleTest_timeOut.csv", false);

	// perform PolyEval to get new values
	cl_float* outputData = PolyEval(coeffsCalculated, order, outputTimeValues, outputSampleSize);
	if (verbose)
		tools::SaveDataFile(outputData, outputSampleSize, 1, "..\\data\\ResampleTest_signalOut.csv", false);

	*coeffs = tools::CopyMatrix(coeffsCalculated, order + 1, 1);

	free(signalData);
	free(inputTimes);
	free(matrixA);
	free(R);
	free(QTranspose);
	free(Qtb);
	free(outputTimeValues);
	free(coeffsCalculated);

	return outputData;
}

int Test_Resample(ResultsStruct* results)
{
	std::cout << "Test Resample: " << std::endl;
	const std::string inputFile = prefs::GetSignalTestDataPath();
	size_t inputRate = prefs::GetTestSampleInputRate();
	size_t outputRate = prefs::GetTestSampleOutputRate();
	size_t order = prefs::GetTestPolynomialOrder();
	cl_float* coeffs = NULL;
	const bool verbose = true; // force printing everything

	ProfilerStruct profiler;
	profiler.Start();
	cl_float* resampledData = Resample(inputFile, inputRate, outputRate, order, &coeffs, verbose);
	profiler.Stop();
	results->HasWindowsRunTime = true;
	results->WindowsRunTime = profiler.Log();

	bool failed = false;
	// Load And Verify Expected Coefficients Results
	std::cout << "Verify Coefficients Match: ";
	{
		const std::string coeffsFile = prefs::GetCoeffsTestDataPath();
		size_t coeffsRows = 0;
		size_t coeffsCols = 0;
		cl_float* expectedCoeffs = tools::LoadDataFile(coeffsFile, &coeffsRows, &coeffsCols);
		if (!expectedCoeffs)
		{
			std::cout << "FAILED to read data file: " << coeffsFile.c_str();
			return -1;
		}
		if (coeffsRows != order + 1)
		{
			std::cout << "FAIL: coefficient count does not match" << std::endl;
			free(expectedCoeffs);
			return -1;
		}
		if (!tools::isEqual<float>(coeffs, expectedCoeffs, order + 1))
		{
			std::cout << "FAIL: coefficients do not match" << std::endl;
			std::cout << "EXPECTED: " << std::endl;
			tools::printMatrix<float>(expectedCoeffs, order + 1, 1);
			std::cout << "ACTUAL: " << std::endl;
			tools::printMatrix<float>(coeffs, order + 1, 1);
			failed = true;
		}
		else
			std::cout << "SUCCESS: Coefficients Match" << std::endl;
		free(expectedCoeffs);
	}

	// Load And Verify Signal Output Results
	std::cout << "Verify Resample Results Match: ";
	{
		const std::string outputFile = prefs::GetOutputTestDataPath();
		size_t outputRows = 0;
		size_t outputCols = 0;
		cl_float* expectedOutput = tools::LoadDataFile(outputFile, &outputRows, &outputCols);
		if (!expectedOutput)
		{
			std::cout << "FAILED to read data file: " << outputFile.c_str();
			return -1;
		}
		if (outputCols != 1)
		{
			std::cout << "FAIL: output data count does not match" << std::endl;
			free(expectedOutput);
			return -1;
		}
		const size_t outputSampleCount = outputRows;
		if (!tools::isEqual<float>(resampledData, expectedOutput, outputSampleCount))
		{
			std::cout << "FAIL: coefficients do not match" << std::endl;
			std::cout << "EXPECTED: " << std::endl;
			tools::printMatrix<float>(expectedOutput, outputSampleCount, 1);
			std::cout << "ACTUAL: " << std::endl;
			tools::printMatrix<float>(resampledData, outputSampleCount, 1);
			failed = true;
		}
		else
			std::cout << "SUCCESS: Resampled Data Matches" << std::endl;
		free(expectedOutput);
	}

	free(coeffs);
	free(resampledData);

	return failed ? -1 : 0;
}

// Calculate Q transpose and R matrixes for QR Decomposition
void QR(cl_float* R, cl_float* Q, size_t arrayWidth, size_t arrayHeight)
{
	cl_float a;
	cl_float b;
	cl_float c;
	cl_float s;
	cl_float r;
	cl_float* Rnew1 = (cl_float*)malloc(arrayWidth * sizeof(cl_float));
	cl_float* Rnew2 = (cl_float*)malloc(arrayWidth * sizeof(cl_float));
	cl_float* Qnew1 = (cl_float*)malloc(arrayHeight * sizeof(cl_float));
	cl_float* Qnew2 = (cl_float*)malloc(arrayHeight * sizeof(cl_float));
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
				Qnew1[k] = Q[arrayHeight * (i - 1) + k] * c - Q[arrayHeight * i + k] * s;
				Qnew2[k] = Q[arrayHeight * (i - 1) + k] * s + Q[arrayHeight * i + k] * c;
			}
			for (size_t k = 0; k < arrayHeight; k++)
			{
				Q[arrayHeight * (i - 1) + k] = Qnew1[k];
				Q[arrayHeight * i + k] = Qnew2[k];
			}
		}
	}
	free(Rnew1);
	free(Rnew2);
	free(Qnew1);
	free(Qnew2);
}

// Tests QR Decomposition
// Will print out Expected and Actual Q and/or R if it fails
int Test_QR(ResultsStruct* results)
{
	std::cout << "Tes QR Decomposition: ";
	const size_t arrayWidth = 3;
	const size_t arrayHeight = 5;

	float A[] = { 1.000000f, 0.000000f,  0.000000f,
				  1.000000f, 1.000000f,  1.000000f,
				  1.000000f, 2.000000f,  4.000000f,
				  1.000000f, 3.000000f,  9.000000f,
				  1.000000f, 4.000000f, 16.000000f };

	// Initialize R
	float* R = tools::CopyMatrix(A, 5, 3);

	// Initialize Q
	float* Q = tools::CreateIdentityMatrix(arrayHeight);

	// Perform QR Decomposition
	ProfilerStruct profiler;
	profiler.Start();
	QR(R, Q, arrayWidth, arrayHeight);
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
	if (!tools::isEqual<float>(R, RExpected, arrayWidth*arrayHeight))
	{
		std::cout << "FAIL" << std::endl;

		std::cout << "A: " << std::endl;
		tools::printMatrix<float>(A, arrayHeight, arrayWidth);
		std::cout << "EXPECTED R: " << std::endl;
		tools::printMatrix<float>(R, arrayHeight, arrayWidth);
		std::cout << "ACTUAL R: " << std::endl;
		tools::printMatrix<float>(RExpected, arrayHeight, arrayWidth);

		failed = true;
	}
	float* QTranspose = tools::TransposeMatrix(Q, arrayHeight, arrayHeight);
	if(!tools::isEqual<float>(QTranspose, QExpected, arrayHeight*arrayHeight))
	{
		std::cout << "FAIL" << std::endl;

		std::cout << "A: " << std::endl;
		tools::printMatrix<float>(A, arrayHeight, arrayWidth);
		std::cout << "EXPECTED Q: " << std::endl;
		tools::printMatrix<float>(QExpected, arrayHeight, arrayHeight);
		std::cout << "ACTUAL Q: " << std::endl;
		tools::printMatrix<float>(QTranspose, arrayHeight, arrayHeight);

		failed = true;
	}
	if(!failed)
		std::cout << "SUCCESS" << std::endl;

	free(QTranspose);
	free(Q);
	free(R);

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
// @return coefficients vector
// NOTE: CALLER TAKES OWNERSHIP OF RETURN VALUE
cl_float* BackSub(cl_float* R, cl_float* Qtb, size_t Dim)
{
	if (!R || !Qtb || Dim == 0)
		return NULL;

	cl_float* result = (cl_float*)malloc(sizeof(cl_float)*Dim);
	for (size_t i = 0; i < Dim; ++i)
		result[i] = 0.0f;

	// Start at the Nth row in the right-triangular matrix R where we effectively have R[n][n] * x[n] = Qtb[n]
	// Work our way up solving for each x[n] value in reverse, allowing us to solve for each row as we work our way up
	for (size_t i = Dim; i > 0; --i)
	{
		const size_t resultIdx = i - 1;
		const size_t RRowStartIdx = resultIdx*Dim; // since R is indexed as an array instead of a matrix

		// sum up product of remaining row of R with known x values
		cl_float subtractSum = 0;
		for (size_t sumIdx = resultIdx + 1; sumIdx < Dim; sumIdx++)
			subtractSum += R[RRowStartIdx + sumIdx] * result[sumIdx];

		// calculate final result eg: if ax + b = c, then x = (c-b)/a
		result[resultIdx] = (Qtb[resultIdx] - subtractSum) / R[RRowStartIdx + resultIdx];
	}
	return result;
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
		float Expected[] = { -24, -13, 2 };

		float* Result = BackSub(R, Qtb, cols);
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

		float Expected[] = { -1.46197f, 2.4261f, -6.87383f, -0.47603f, 2.34757f };

		float* Result = BackSub(R, Qtb, cols);
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
// @return output array of calculated values
cl_float* PolyEval(cl_float* coeffs, size_t order, cl_float* input, size_t numSamples)
{
	if (!coeffs || !order || !input || !numSamples)
		return NULL;

	cl_float* output = (cl_float*)malloc(sizeof(cl_float)*numSamples);
	for (size_t i = 0; i < numSamples; ++i)
		output[i] = 0.0f;

	// for each data point
	for (size_t valIdx = 0; valIdx < numSamples; ++valIdx)
	{
		output[valIdx] = 0;
		// sum: a0*x^0 + a1*x^1 + a2*x^2 + ... + an*x^n
		for (size_t coef = 0; coef <= order; ++coef)
			output[valIdx] += coeffs[coef] * (float)pow(input[valIdx], coef);
	}
	return output;
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
		float Expected[] = { 1, 3, 7, 13, 21, 31 };

		float* Result = PolyEval(coeffs, order, input, numSamples);
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
		free(Result);
	}
	std::cout << std::endl << "Simple Test 2: ";
	{
		float coeffs[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
		const size_t order = 7;

		float input[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
		const size_t numSamples = 10;

		float Expected[] = { 36, 1793, 24604, 167481, 756836, 2620201, 7526268, 18831568, 42374116, 87654320 };
		float* Result = PolyEval(coeffs, order, input, numSamples);
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
		free(Result);
	}

	return 0;
}

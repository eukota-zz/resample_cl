#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <memory.h>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

//for perf. counters
#include <Windows.h>

#define pi 3.141592654

/*// Solves for coefficients in equation R*x=Q*b where
// R and Q are from QR Decomposition
// x is desired coefficients
// b is column vector of sampled data
// @param[in] R upper triangular matrix from QR Decomposition
// @param[in] Qtb Q*b column vector
// @param[in] rows number of sample data points
// @param[in] cols number of coefficients to solve for
// @param[out] Result is where x is stored
void BackSub(float* R, float* Qtb, int dim, float* Result)
{
	if (!Result)
		Result = (float*)malloc(sizeof(float*)*dim);

	// solve for last last value without any subtraction
	Result[dim - 1] = Qtb[dim - 1] / R[dim*dim - 1];
	int count = dim - 2;
	for (int i = dim - 2; i >= 0; i--)
	{
		int from = i + 1;
		int to = dim;

		float subtractSum = 0;
		for (int j = from; j < to; j++)
			subtractSum += R[i*dim + j] * Result[j];

		Result[i] = (Qtb[i] - subtractSum) / R[i*dim + count];
		count -= 1;
	}
}*/

// Solves for coefficients in equation R*x=Q*b where
// R and Q are from QR Decomposition
// x is desired coefficients
// b is column vector of sampled data
// @param[in] R upper triangular matrix of size MxN (from QR Decomposition)
// @param[in] Qtb Q*b column vector of size Mx1
// @param[in] Dim coefficients to solve for - should match N columns size of R
// @param[out] Result is where x is stored
void BackSub(float* R, float* Qtb, size_t Dim, float* Result)
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
		float subtractSum = 0;
		for (size_t sumIdx = resultIdx + 1; sumIdx < Dim; sumIdx++)
			subtractSum += R[RRowStartIdx + sumIdx] * Result[sumIdx];

		// calculate final result eg: if ax + b = c, then x = (c-b)/a
		if (Result)
			Result[resultIdx] = (Qtb[resultIdx] - subtractSum) / R[RRowStartIdx + resultIdx];
	}
}

void mat_mul(int N, float *A, float *B, float *C)
{
	int i, j, k;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			C[i*N + j] = 0.0f;
			for (k = 0; k < N; k++) {
				// C(i, j) = sum(over k) A(i,k) * B(k,j)
				C[i*N + j] += A[i*N + k] * B[k*N + j];
			}
		}
	}
}

// Calculate Q and R matrixes for QR Decomposition of matrix. Outputs R and Q transpose
void QR(float* R, float* Qt, int arrayWidth, int arrayHeight)
{
	float a;
	float b;
	float c;
	float s;
	float r;
	float Rnew1[2048];
	float Rnew2[2048];
	float Qnew1[2048];
	float Qnew2[2048];
	int rowG;
	int colG;
	for (int j = 0; j < arrayWidth; j++)
	{
		rowG = arrayHeight - 1;
		colG = arrayHeight - 1;
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
				Qnew1[k] = Qt[arrayHeight * (i - 1) + k] * c - Qt[arrayHeight * i + k] * s;
				Qnew2[k] = Qt[arrayHeight * (i - 1) + k] * s + Qt[arrayHeight * i + k] * c;
			}
			for (int k = 0; k < arrayHeight; k++)
			{
				Qt[arrayHeight * (i - 1) + k] = Qnew1[k];
				Qt[arrayHeight * i + k] = Qnew2[k];
			}
		}
	}
}

/*
* Function tGen generates time vectors for the input data and output data
* of the resampler.
*
* Inputs:
*    inFs       -- Input sample rate
*    outFs      -- Output sample rate
*    numSamples -- Number of samples to generate
*
* Outputs:
* tIn        -- Generated time vector for the input data
* tOut       -- Generated time vector for the output data
* A          -- Generated A matrix with tIn values
*/
void tGen(float inFs, float outFs, int k, int numSamples, float* tIn, float* tOut, float* A)
{
	for (int i = 0; i < numSamples; i++)
	{
		tIn[i] = i;
		tOut[i] = i * inFs / outFs;
	}

	// Construct A matrix for use in QR decomposition
	for (int m = 0; m < numSamples; m++)
	{
		for (int n = 0; n <= k; n++)
		{
			A[m*(k+1) + n] = pow(tIn[m], n);
		}
	}
}

/*
* Function sigGen generates sinusoidal complex IQ data for the resampler
*
* Inputs:
*    Fs         -- Sample rate
*    f          -- Frequency of the generated sine and cosine waves
*    noise      -- Controls the level of noise added to the signal (0 is no noise)
*    numSamples -- Number of samples to generate
* Outputs:
*    I          -- Real component of the generated signal
*    Q          -- Imaginary component of the generated signal
*/
void sigGen(float Fs, float f, float noise, float numSamples, float* I, float* Q)
{
	float t;
	for (int i = 0; i < numSamples; i++)
	{
		t = i * 1 / Fs;
		I[i] = cos(2 * pi * f * t) + noise * (2 * (float)rand() / (float)RAND_MAX - 1);
		Q[i] = sin(2 * pi * f * t) + noise * (2 * (float)rand() / (float)RAND_MAX - 1);
	}
}

void polyEval(float* coef, float* tOut, float* sig, int k, int numSamples)
{
	for (int i = 0; i <= k; i++)
	{
		for (int j = 0; j < numSamples; j++)
		{
			sig[j] += coef[i] * pow(tOut[j], i);
		}
	}
}


// Matrix multiplication where A=NxN, B=Nx1, and C=Nx1
void matrixMult(int N, float *A, float *B, float *C)
{
	int i, j, k;
	for (i = 0; i < N; i++) {
		C[i] = 0.0f;
		for (j = 0; j < N; j++) {
			C[i] += A[i*N + j] * B[j];
		}
	}
}

void resample(float inFs, float outFs, int numSamples, float* sigIn, float* sigOut, float* tIn, float* tOut, float* Qt, float* Qtb, float* R, float* coef, int k)
{

	// Generate time vectors
	tGen(inFs, outFs, k, numSamples, tIn, tOut, R);
#ifdef debug
	printf("A=\n");
	for (int i = 0; i < numSamples; i++)
	{
		printf("[");
		for (int j = 0; j <= k; j++)
		{
			printf("%f ", R[i*(k+1) + j]);
		}
		printf("]\n");
	}
#endif
	// QR decomposition
	QR(R, Qt, k+1, numSamples);
#ifdef debug
	printf("R=\n");
	for (int i = 0; i < numSamples; i++)
	{
		printf("[");
		for (int j = 0; j < k; j++)
		{
			printf("%f ", R[i*k + j]);
		}
		printf("]\n");
	}

	printf("\nQ = \n");
	for (int i = 0; i < numSamples; i++)
	{
		printf("[");
		for (int j = 0; j < numSamples; j++)
		{
			printf("%f ", Qt[i * numSamples + j]);
		}
		printf("]\n");
	}
#endif
	// Solve for Q'*b
	matrixMult(numSamples, Qt, sigIn, Qtb);

	// Back substitution to solve for polynomial coefficients
	BackSub(R, Qtb, k+1, coef);

	// Matrix multiply to get Q'*B

	// Polynomial reconstruction of input signal
	polyEval(coef, tOut, sigOut, k, numSamples);
}

int main()
{
	float inFs = 1;
	float outFs = 1;

	const int numSamples = 100;
	const int k = 5; // Polynomial order

	float x[numSamples];
	float sigIn[numSamples];
	float sigTmp[numSamples];
	float sigOut[numSamples];

	float sig[numSamples];
	float tIn[numSamples];
	float tOut[numSamples];
	float Qt[numSamples*numSamples];
	float Qtb[numSamples];
	float R[(k+1)*numSamples];
	float coef[k+1];

	// Initialize x
	for (int i = 0; i < numSamples; i++)
	{
		x[i] = i;
	}

    // Initialize sigOut
	for (int i = 0; i < numSamples; i++)
	{
		sigIn[i] = sin(2 * pi * x[i] / 20);
		sigOut[i] = 0;
	}
	
	// Initialize sigIn
	sigGen(10e6, 100e3, 0.2, numSamples, sigTmp, sigIn);

	// Initialize Qt and G
	for (int i = 0; i < numSamples; i++)
	{
		for (int j = 0; j < numSamples; j++)
		{
			if (i == j)
			{
				Qt[i*numSamples + j] = 1;
			}
			else
			{
				Qt[i*numSamples + j] = 0;
			}
		}
	}

	// Initialize R
	for (int i = 0; i < k * numSamples; i++)
	{
		R[i] = 0;
	}

	resample(inFs, outFs, numSamples, sigIn, sigOut, tIn, tOut, Qt, Qtb, R, coef, k);
#ifdef debug
	// Print output signal
	for (int i = 0; i < numSamples; i++)
	{
		printf("%f, %f\n", tOut[i], sigOut[i]);
	}
#endif
	// Write data to file
	ofstream outfile;
	outfile.open("data.csv");
	for (int i = 0; i < numSamples; i++)
	{
		outfile << tIn[i] << "," << sigIn[i] << "," << tOut[i] << "," << sigOut[i] << "\n";
	}
	outfile.close();
	
	return 0;
}
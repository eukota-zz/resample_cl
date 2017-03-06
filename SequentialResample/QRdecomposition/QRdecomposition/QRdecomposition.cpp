/*****************************************************************************
 * Copyright (c) 2013-2016 Intel Corporation
 * All rights reserved.
 *
 * WARRANTY DISCLAIMER
 *
 * THESE MATERIALS ARE PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR ITS
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THESE
 * MATERIALS, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Intel Corporation is the author of the Materials, and requests that all
 * problem reports or change requests be submitted to it directly
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <memory.h>
#include <vector>

//for perf. counters
#include <Windows.h>


// Macros for OpenCL versions
#define OPENVERSION_1_2  1.2f
#define OPENVERSION_2_0  2.0f

#define PRINT_RESULTS
//define GET_TIMING

/*
 * Sequential QR decomposition function
 */
void QR(float* R, float* Q, int arrayWidth, int arrayHeight)
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


/*
 * main execution routine
 * Basically it consists of three parts:
 *   - generating the inputs
 *   - running OpenCL kernel
 *   - reading results of processing
 */
int _tmain(int argc, TCHAR* argv[])
{
    LARGE_INTEGER perfFrequency;
    LARGE_INTEGER performanceCountNDRangeStart;
    LARGE_INTEGER performanceCountNDRangeStop;

    const int arrayWidth  = 3;
    const int arrayHeight = 5;
	int numIter = 100; // Number of iterations for runtime averaging

	// allocate working buffers. 
	// the buffer should be aligned with 4K page and size should fit 64-byte cached line
	int optimizedSize = ((sizeof(float) * arrayWidth * arrayHeight - 1) / 64 + 1) * 64;
	float* A = (float*)_aligned_malloc(optimizedSize, 4096);

	optimizedSize = ((sizeof(float) * arrayHeight * arrayHeight - 1) / 64 + 1) * 64;
	float* Q = (float*)_aligned_malloc(optimizedSize, 4096);

	/*float Atmp[] = { 0.8147, 0.0975, 0.1576,
				        0.9058, 0.2785, 0.9706,
					    0.1270, 0.5469, 0.9572,
					    0.9134, 0.9575, 0.4854,
					    0.6324, 0.9649, 0.8003 };*/

	/*float Atmp[] = { 0.005982, 0.563494, 0.853145, 0.245674, 0.339763,
					   0.573901, 0.827815, 0.181158, 0.324046, 0.141362,
					   0.886715, 0.753594, 0.259835, 0.673940, 0.372814,
					   0.733390, 0.937681, 0.993835, 0.737785, 0.510269,
					   0.454573, 0.511582, 0.145054, 0.908719, 0.892514 };*/

	float Atmp[] = { 1.000000, 0.000000, 0.000000,
					 1.000000, 1.000000, 1.000000,
					 1.000000, 2.000000, 4.000000,
					 1.000000, 3.000000, 9.000000,
					 1.000000, 4.000000, 16.000000 };
   
    if (NULL == A)
    {
        printf("Error: _aligned_malloc failed to allocate buffers.\n");
        return -1;
    }

	// Initialize A
	for (int i = 0; i < arrayWidth * arrayHeight; i++)
	{
		//A[i] = rand();
		A[i] = Atmp[i];
	}

	// Initialize Q
	for (int i = 0; i < arrayHeight; i++)
	{
		for (int j = 0; j < arrayHeight; j++)
		{
			if (i == j)
			{
				Q[i * arrayHeight + j] = 1;
			}
			else
			{
				Q[i * arrayHeight + j] = 0;
			}
		}
	}
#ifdef PRINT_RESULTS
	// Print A
	printf("\nA = \n");
	for (int i = 0; i < arrayHeight; i++)
	{
		printf("[");
		for (int j = 0; j < arrayWidth; j++)
		{
			printf("%f ", A[arrayWidth * i + j]);
		}
		printf("]\n");
	}

	QR(A, Q, arrayWidth, arrayHeight);

	// Print results
	printf("\nR = \n");
	for (int i = 0; i < arrayHeight; i++)
	{
		printf("[");
		for (int j = 0; j < arrayWidth; j++)
		{
			printf("%f ", A[arrayWidth * i + j]);
		}
		printf("]\n");
	}

	printf("\nQ = \n");
	for (int i = 0; i < arrayHeight; i++)
	{
		printf("[");
		for (int j = 0; j < arrayHeight; j++)
		{
			printf("%f ", Q[i + arrayHeight * j]);
		}
		printf("]\n");
	}
#endif
#ifdef GET_TIMING
    bool queueProfilingEnable = true;
    if (queueProfilingEnable)
        QueryPerformanceCounter(&performanceCountNDRangeStart);

	for (int i = 0; i < numIter; i++)
	{
		QR(A, Q, arrayWidth, arrayHeight);
	}

   
    if (queueProfilingEnable)
        QueryPerformanceCounter(&performanceCountNDRangeStop);

	_aligned_free(A);

	// retrieve performance counter frequency
	if (queueProfilingEnable)
	{
		QueryPerformanceFrequency(&perfFrequency);
		printf("NDRange performance counter average time %f ms. for %d iterations.\n",
			1000.0f*(float)(performanceCountNDRangeStop.QuadPart - performanceCountNDRangeStart.QuadPart) / (float)perfFrequency.QuadPart / numIter, numIter);
	}
#endif
    return 0;
}


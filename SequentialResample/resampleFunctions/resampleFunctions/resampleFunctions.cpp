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
#include <math.h>

//for perf. counters
#include <Windows.h>

#define pi 3.141592654

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
 */
void tGen(float inFs, float outFs, int numSamples, float* tIn, float* tOut)
{
	for (int i = 0; i < numSamples; i++)
	{
		tIn[i] = i;
		tOut[i] = i * inFs / outFs;
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
		I[i] = cos(2 * pi * f * t) + noise * (2 * (float)rand()/(float)RAND_MAX - 1);
		Q[i] = sin(2 * pi * f * t) + noise * (2 * (float)rand()/(float)RAND_MAX - 1);
	}
}

int _tmain(int argc, TCHAR* argv[])
{
	const int numSamples = 100;
	float tIn[numSamples];
	float tOut[numSamples];
	float inFs = 1;
	float outFs = 2;

	float Fs = 10;
	float f = 1;
	float noise = 1;
	float I[numSamples];
	float Q[numSamples];

	// Generate time vectors

	tGen(inFs, outFs, numSamples, tIn, tOut);

	printf("tIn:\n");
	for (int i = 0; i < numSamples; i++)
		printf("%f ", tIn[i]);
	printf("\n\ntOut:\n");
	for (int i = 0; i < numSamples; i++)
		printf("%f ", tOut[i]);

	// Generate test signal
	sigGen(Fs, f, noise, numSamples, I, Q);

	printf("\n\nI:\n");
	for (int i = 0; i < numSamples; i++)
		printf("%f ", I[i]);
	printf("\n\Q:\n");
	for (int i = 0; i < numSamples; i++)
		printf("%f ", Q[i]);

    return 0;
}


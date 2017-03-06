#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <memory.h>
#include <vector>

//for perf. counters
#include <Windows.h>

#define pi 3.141592654

void polyEval(float* coef, float* tOut, float* sig, int k, int numPoints)
{
	for (int i = 0; i <= k; i++)
	{
		for (int j = 0; j < numPoints; j++)
		{
			sig[j] += coef[i] * pow(tOut[j], i);
		}
	}
}

int main()
{
	float coef[8] = { 1, 2, 3, 4, 5, 6, 7, 8};
	float tOut[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	float sig[10];
	int k = 7;
	int numPoints = 10;

	// Initialize sig to 0
	for (int i = 0; i < numPoints; i++)
	{
		sig[i] = 0;
	}

	polyEval(coef, tOut, sig, k, numPoints);

	for (int i = 0; i < numPoints; i++)
	{
		printf("%f ", sig[i]);
	}
	printf("\n");

	return 0;
}
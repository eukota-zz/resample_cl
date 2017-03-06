#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <memory.h>
#include <vector>

//for perf. counters
#include <Windows.h>

#define pi 3.141592654

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

int main()
{
	float A[] = { 1,2,3,
				  4,5,6,
				  7,8,9 };

	float B[] = { 1, 2, 3 };
	float C[3];

	matrixMult(3, A, B, C);

	for (int i = 0; i < 3; i++)
	{
		printf("%f\n", C[i]);
	}
	return 0;
}
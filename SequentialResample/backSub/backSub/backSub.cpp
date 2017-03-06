#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <memory.h>
#include <vector>

//for perf. counters
#include <Windows.h>

// Solves for coefficients in equation R*x=Q*b where
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
	Result[dim-1] = Qtb[dim-1] / R[dim*dim-1];
	int count = dim-2;
	for (int i = dim-2; i >= 0; i--)
	{
		int from = i + 1;
		int to = dim;

		float subtractSum = 0;
		for (int j = from; j < to; j++)
			subtractSum += R[i*dim+j] * Result[j];

		Result[i] = (Qtb[i] - subtractSum) / R[i*dim+count];
		count -= 1;
	}
}

int main()
{
	const int cols = 5;

	float Qtb[] = { 0.74882, 0.95545, 0.37916, 0.84571, 0.71664 };

	float R[] = { 0.845668,   0.776442,   0.049804,   0.223160,   0.234288,
		          0.321790,   0.953842,   0.344031,   0.673007,   0.565058,
		          0.041129,   0.496991,   0.221390,   0.170497,   0.844328,
		          0.814968,   0.691170,   0.545236,   0.780255,   0.518466,
		          0.863745,   0.461225,   0.834668,   0.176055,   0.305269 };

	/*float Qtb[] = {4, -1, 2};
	float R[] = { 1, -2, 1,
				 0,  1, 6,
				 0,  0,  1 };*/

	float Result[cols];


	BackSub(R, Qtb, cols, Result);

	for (int i = 0; i < cols; i++)
	{
		printf("x%d %f\n", i, Result[i]);
	}

	return 0;
}
__kernel void PolyEvalOcl(float8 coeffs, __global float* t, __global float* result)
{
	const unsigned int id = get_global_id(0);
	float val = t[id];
	float out = 0;
	for(unsigned int i=0; i<7; ++i)
		out += coeffs[i]*pow(val,i);

	result[id] = out;
}

// MatrixMultiplierOcl takes an MxN matrix A and multiplies it by a Nx1 matrix B (matrixC = matrixA * matrixB)
__kernel void MatrixMultiplierOcl(int colsA, __global float* matrixA, __global float* matrixB, __global float* matrixC)
{
	int k;
	const unsigned int n = get_global_id(0);
	float tmp = 0.0f;
	for (k = 0; k < colsA; k++)
    {
		tmp += matrixA[n * colsA + k] * matrixB[k];
	}
	matrixC[n] = tmp;
}
	
	
// returns [A]*[B] = [C]
// Assumption is that A = MxN and B = NxL and C = MxL
__kernel void MatrixMultiplier(__global float* matrixA, __global float* matrixB, __global float* matrixC, unsigned int widthA, unsigned int widthB)
{
	const unsigned int rowC = get_global_id(0); // M
	const unsigned int colC = get_global_id(1); // L

	float result = 0.0f;
	for(unsigned int inner = 0; inner < widthA; ++inner)
		result += matrixA[rowC*widthA + inner]*matrixB[inner*widthB + colC];

	matrixC[rowC*widthB+colC] = result;
	//printf("running for (%d,%d): %f\n", rowC, colC, result);
}
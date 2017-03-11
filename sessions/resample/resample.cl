
// evaluates a polynomial with provided coefficients
// eg: result[0] = a0*t^0 + a1*t^1 + a2*t^2 + .... an*t^n
__kernel void PolyEvalOcl(__global float* coeffs, unsigned int order, __global float* input, __global float* result)
{
	const unsigned int id = get_global_id(0);
	float val = input[id];
	float out = 0;
	for(unsigned int i=0; i<=order; ++i)
		out += coeffs[i]*pow(val,i);

	result[id] = out;
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
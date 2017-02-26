__kernel void Resample(float8 coeffs, __global float* t, __global float* result)
{
	const unsigned int id = get_global_id(0);
	float val = t[id];
	float out = 0;
	for(unsigned int i=0; i<8; ++i)
		out += coeffs[i]*pow(val,i);

	result[id] = out;
}


__kernel void progressiveArraySum(__global float* p1b_A, __global float* p1b_B)
{
	const unsigned int id = get_global_id(0);

	float out = 0.0;
	for(unsigned int i=0; i<=id; i++)
		out += p1b_A[i];

	p1b_B[id] = out;
}


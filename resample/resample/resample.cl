__kernel void Resample(float8 coeffs, __global float* t, __global float* result)
{
	const unsigned int id = get_global_id(0);
	float val = t[id];
	float out = 0;
	for(unsigned int i=0; i<7; ++i)
		out += coeffs[i]*pow(val,i);

	result[id] = out;
}



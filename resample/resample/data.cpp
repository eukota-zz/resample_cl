#include "data.h"
#include "stdlib.h"

namespace data
{
	void generateInputCL(cl_float* inputArray, cl_uint arrayWidth, cl_uint arrayHeight)
	{
		// random initialization of input
		size_t array_size = arrayWidth * arrayHeight;
		for (size_t i = 0; i < array_size; ++i)
		{
			inputArray[i] = (cl_float)(rand() % 100); // mod to fix possible floating point comparison issues found during office hours with Logan
		}
	}

	void generateInputCLSeq(cl_float* inputArray, cl_uint arrayWidth, cl_uint arrayHeight)
	{
		size_t array_size = arrayWidth * arrayHeight;
		for (size_t i = 0; i < array_size; ++i)
		{
			inputArray[i] = 1;
		}
	}

}
#pragma once
#include "CL/cl.h"
namespace data
{

	void generateInputCL(cl_float* inputArray, cl_uint arrayWidth, cl_uint arrayHeight);
	void generateInputCLSeq(cl_float* inputArray, cl_uint arrayWidth, cl_uint arrayHeight);

}
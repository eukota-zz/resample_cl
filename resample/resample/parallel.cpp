#include "parallel.h"
#include "groups.h"
#include "ocl.h"
#include "utils.h"
#include "enums.h"
#include "data.h"
#include "profiler.h"
#include "CL/cl.h"

namespace
{
	const char* FILENAME = "resample.cl";
}

////////////////// RESAMPLE USING POLYNOMIAL APPROXIMATION /////////////////
int exCL_Resample(ResultsStruct* results)
{
	cl_int err;
	ocl_args ocl(CL_DEVICE_TYPE_GPU);

	// Create Local Variables and Allocate Memory
	// The buffer should be aligned with 4K page and size should fit 64-byte cached line
	cl_uint sampleSize = 1024;
	cl_uint optimizedSizeFloat = ((sizeof(cl_float) * sampleSize - 1) / 64 + 1) * 64;
	cl_float* inputA = (cl_float*)_aligned_malloc(optimizedSizeFloat, 4096);
	cl_float* outputC = (cl_float*)_aligned_malloc(optimizedSizeFloat, 4096);
	if (NULL == inputA || NULL == outputC)
	{
		LogError("Error: _aligned_malloc failed to allocate buffers.\n");
		return -1;
	}
	// Generate Random Input
	data::generateInputCLSeq(inputA, sampleSize, 1);

	// Create OpenCL buffers from host memory for use by Kernel
	cl_float8        coeffs = { 1,2,3,4,5,6,7,0 };
	cl_mem           srcA;              // hold first source buffer
	cl_mem           dstMem;            // hold destination buffer
	if (CL_SUCCESS != CreateReadBufferArg(&ocl.context, &srcA, inputA, sampleSize, 1))
		return -1;
	if (CL_SUCCESS != CreateWriteBufferArg(&ocl.context, &dstMem, outputC, sampleSize, 1))
		return -1;

	// Create and build the OpenCL program - imports named cl file.
	if (CL_SUCCESS != ocl.CreateAndBuildProgram(FILENAME))
		return -1;

	// Create Kernel - kernel name must match kernel name in cl file
	ocl.kernel = clCreateKernel(ocl.program, "Resample", &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clCreateKernel returned %s\n", TranslateOpenCLError(err));
		return -1;
	}

	// Set OpenCL Kernel Arguments - Order Indicated by Final Argument
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &coeffs, 0))
		return -1;
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &srcA, 1))
		return -1;
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &dstMem, 2))
		return -1;

	// Enqueue Kernel (wrapped in profiler timing)
	ProfilerStruct profiler;
	profiler.Start();
	size_t globalWorkSize[1] = { sampleSize };
	// hard code work group size after finding optimal solution with KDF Sessions
	size_t localWorkSize[1] = { 16 };
	if (CL_SUCCESS != ocl.ExecuteKernel(globalWorkSize, 1, localWorkSize))
		return -1;
	profiler.Stop();
	float runTime = profiler.Log();

	if (!SKIP_VERIFICATION)
	{
		// Map Host Buffer to Local Data
		cl_float* resultPtr = NULL;
		if (CL_SUCCESS != MapHostBufferToLocal(&ocl.commandQueue, &dstMem, sampleSize, 1, &resultPtr))
		{
			LogError("Error: clEnqueueMapBuffer failed.\n");
			return -1;
		}

		// VERIFY DATA
		// We mapped dstMem to resultPtr, so resultPtr is ready and includes the kernel output !!!
		// Verify the results
		bool failed = false;
		/// @TODO WRITE SEQUENTIAL VERIFICATION CODE
		/*
		float cumSum = 0.0;
		for (size_t i = 0; i < sampleSize; ++i)
		{
		cumSum += inputA[i];
		if (resultPtr[i] != cumSum)
		{
		LogError("Verification failed at %d: Expected: %f. Actual: %f.\n", i, cumSum, resultPtr[i]);
		failed = true;
		}
		}
		*/
		if (!failed)
			LogInfo("Verification passed.\n");

		// Unmap Host Buffer from Local Data
		if (CL_SUCCESS != UnmapHostBufferFromLocal(&ocl.commandQueue, &dstMem, resultPtr))
			LogInfo("UnmapHostBufferFromLocal Failed.\n");
	}

	_aligned_free(inputA);
	_aligned_free(outputC);

	if (CL_SUCCESS != clReleaseMemObject(srcA))
		LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));
	if (CL_SUCCESS != clReleaseMemObject(dstMem))
		LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));

	results->WindowsRunTime = runTime;
	results->HasWindowsRunTime = true;
	results->OpenCLRunTime = ocl.RunTimeMS();
	results->HasOpenCLRunTime = true;
	return 0;
}

////////////////// QR DECOMPOSITION ///////////////
int exCL_QRD(ResultsStruct* results)
{
	return 0;
}

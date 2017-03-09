#include "parallel.h"
#include "groups.h"
#include "ocl.h"
#include "utils.h"
#include "tools.h"
#include "enums.h"
#include "data.h"
#include "profiler.h"
#include "CL/cl.h"
#include "constants.h"

namespace
{
	const char* FILENAME = "resample.cl";
}

// Evaluate polynomial results for input values with coefficients in parallel
// @param[in] coeffs array of coefficients of size order+1
// @param[in] order order of the polynomial
// @param[in] input array of input values
// @param[in] numSamples number of input values
// @return output array of calculated values
cl_float* PolyEvalOcl(cl_float* coeffs, size_t order, cl_float* input, size_t numSamples)
{
	cl_int err;
	ocl_args ocl(CL_DEVICE_TYPE_GPU);

	cl_float* output = (cl_float*)malloc(numSamples * sizeof(cl_float));

	// Create OpenCL buffers from host memory for use by Kernel
	cl_float8        c = { coeffs[0], coeffs[1], coeffs[2], coeffs[3], coeffs[4], coeffs[5], coeffs[6], coeffs[7] };
	cl_mem           srcA;              // hold first source buffer
	cl_mem           dstMem;            // hold destination buffer
	if (CL_SUCCESS != CreateReadBufferArg(&ocl.context, &srcA, input, (cl_uint)numSamples, 1))
		return 0;
	if (CL_SUCCESS != CreateWriteBufferArg(&ocl.context, &dstMem, output, (cl_uint)numSamples, 1))
		return 0;

	// Create and build the OpenCL program - imports named cl file.
	if (CL_SUCCESS != ocl.CreateAndBuildProgram(FILENAME))
		return 0;

	// Create Kernel - kernel name must match kernel name in cl file
	ocl.kernel = clCreateKernel(ocl.program, "PolyEvalOcl", &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clCreateKernel returned %s\n", TranslateOpenCLError(err));
		return 0;
	}

	// Set OpenCL Kernel Arguments - Order Indicated by Final Argument
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &c, 0))
		return 0;
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &srcA, 1))
		return 0;
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &dstMem, 2))
		return 0;

	// Enqueue Kernel
	size_t globalWorkSize[1] = { numSamples };
	if (CL_SUCCESS != ocl.ExecuteKernel(globalWorkSize, 1, NULL))
		return 0;

	// Map Host Buffer to Local Data
	if (CL_SUCCESS != MapHostBufferToLocal(&ocl.commandQueue, &dstMem, (cl_uint)numSamples, 1, &output))
	{
		LogError("Error: clEnqueueMapBuffer failed.\n");
		return 0;
	}

	if (CL_SUCCESS != clReleaseMemObject(srcA))
		LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));
	if (CL_SUCCESS != clReleaseMemObject(dstMem))
		LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));

    return output;
}

// Multiplies two matrices together [A]*[B] where A is MxN and B is Nx1
// @param[in] rowsA number of rows of A (M)
// @param[in] colsA number of columns of A (N)
// @param[in] matrixA left matrix
// @param[in] matrixB right colunm vector
// @return output result
// NOTE: CALLER TAKES OWNERSHIP OF RETURN VALUE
float* MatrixMultiplierOcl(cl_uint rowsA, cl_uint colsA, float* matrixA, float* matrixB)
{
	cl_int err;
	ocl_args ocl(CL_DEVICE_TYPE_GPU);

	cl_float* matrixC = (cl_float*)malloc(colsA * sizeof(cl_float));

	// Create OpenCL buffers from host memory for use by Kernel
	cl_mem           srcA;              // hold the A matrix buffer
	cl_mem           srcB;              // holds the B matrix buffer
	cl_mem           dstMem;            // hold the C matrix buffer
	if (CL_SUCCESS != CreateReadBufferArg(&ocl.context, &srcA, matrixA, rowsA * colsA, 1))
		return 0;
	if (CL_SUCCESS != CreateReadBufferArg(&ocl.context, &srcB, matrixB, colsA, 1))
		return 0;
	if (CL_SUCCESS != CreateWriteBufferArg(&ocl.context, &dstMem, matrixC, colsA, 1))
		return 0;

	// Create and build the OpenCL program - imports named cl file.
	if (CL_SUCCESS != ocl.CreateAndBuildProgram(FILENAME))
		return 0;

	// Create Kernel - kernel name must match kernel name in cl file
	ocl.kernel = clCreateKernel(ocl.program, "MatrixMultiplierOcl", &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clCreateKernel returned %s\n", TranslateOpenCLError(err));
		return 0;
	}

	// Set OpenCL Kernel Arguments - Order Indicated by Final Argument
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &colsA, 0))
		return 0;
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &srcA, 1))
		return 0;
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &srcB, 2))
		return 0;
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &dstMem, 3))
		return 0;

	// Enqueue Kernel
	size_t globalWorkSize[1] = { colsA };
	if (CL_SUCCESS != ocl.ExecuteKernel(globalWorkSize, 1, NULL))
		return 0;

	// Map Host Buffer to Local Data
	if (CL_SUCCESS != MapHostBufferToLocal(&ocl.commandQueue, &dstMem, colsA, 1, &matrixC))
	{
		LogError("Error: clEnqueueMapBuffer failed.\n");
		return 0;
	}

	if (CL_SUCCESS != clReleaseMemObject(srcA))
		LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));
	if (CL_SUCCESS != clReleaseMemObject(dstMem))
		LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));

	return matrixC;
}

int Test_PolyEvalOcl(ResultsStruct* results)
{
	cl_float coeffs[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
	cl_float input[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f };
	cl_float Expected[] = { 36.0f, 1793.0f, 24604.0f, 167481.0f, 756836.0f, 2620201.0f, 7526268.0f, 18831569.0f, 42374116.0f, 87654321.0f };
	cl_int numSamples = 10;
	int order = 7;

	ProfilerStruct profiler;
	profiler.Start();

	cl_float* Results = PolyEvalOcl(coeffs, order, input, numSamples);

	profiler.Stop();
	float runTime = profiler.Log();

	if (tools::isEqual<float>(Results, Expected, numSamples))
	{
		std::cout << "SUCCESS!" << std::endl;
	}
	else
	{
		std::cout << "FAILURE!" << std::endl;
		std::cout << "Expected: " << std::endl;
		tools::printArray<float>(Expected, numSamples);
		std::cout << "Actual: " << std::endl;
		tools::printArray<float>(Results, numSamples);
	}

	results->WindowsRunTime = runTime;
	results->HasWindowsRunTime = true;
	results->OpenCLRunTime = 0;
	results->HasOpenCLRunTime = false;
	return 0;
}

int Test_MatrixMultiplierOcl(ResultsStruct* results)
{
	cl_uint rowsA = 3;
	cl_uint colsA = 3;
	cl_float matrixA[] = { 1, 2, 3,
					       4, 5, 6,
						   7, 8, 9 };

	cl_float matrixB[3] = { 1, 2, 3 };

	cl_float Expected[] = { 14, 32, 50 };

	ProfilerStruct profiler;
	profiler.Start();

	cl_float* Results = MatrixMultiplierOcl(rowsA, colsA, matrixA, matrixB);

	profiler.Stop();
	float runTime = profiler.Log();

	if (tools::isEqual<float>(Results, Expected, colsA))
	{
		std::cout << "SUCCESS!" << std::endl;
	}
	else
	{
		std::cout << "FAILURE!" << std::endl;
		std::cout << "Expected: " << std::endl;
		tools::printArray<float>(Expected, colsA);
		std::cout << "Actual: " << std::endl;
		tools::printArray<float>(Results, colsA);
	}

	results->WindowsRunTime = runTime;
	results->HasWindowsRunTime = true;
	results->OpenCLRunTime = 0;
	results->HasOpenCLRunTime = false;
	return 0;
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

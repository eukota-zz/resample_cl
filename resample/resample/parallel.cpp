#include "parallel.h"
#include "groups.h"
#include "ocl.h"
#include "utils.h"
#include "tools.h"
#include "enums.h"
#include "data.h"
#include "profiler.h"
#include "CL/cl.h"
#include "settings.h"

// Evaluate polynomial results for input values with coefficients in parallel
// @param[in] coeffs array of coefficients of size order+1
// @param[in] order order of the polynomial 
// @param[in] input array of input values
// @param[in] numSamples number of input values
// @return output array of calculated values
// NOTE: CALLER TAKES OWNERSHIP OF RETURN VALUE
cl_float* PolyEvalOcl(cl_float* coeffs, cl_uint order, cl_float* input, cl_uint numSamples)
{
	cl_int err;
	ocl_args ocl(CL_DEVICE_TYPE_GPU);

	// Create Local Variabels and Allocate Memory
	cl_uint optimizedSize = ((sizeof(cl_float)*numSamples - 1) / 64 + 1) * 64;
	cl_float* outputSignalData = (cl_float*)_aligned_malloc(optimizedSize, 4096);
	// coefficients are function argument
	// input is function argument
	if (!coeffs || !order || !input || !numSamples)
	{
		LogError("Error: invalid data input)");
		return NULL;
	}

	// Create OpenCL buffers from host memory for use by Kernel
	cl_mem           srcCoefficients;
	cl_mem           srcOutputTimes;              // hold first source buffer
	cl_mem           dstOutputSignal;            // hold destination buffer
	if (CL_SUCCESS != CreateReadBufferArg(&ocl.context, &srcCoefficients, coeffs, order+1, 1))
		return NULL;
	if (CL_SUCCESS != CreateReadBufferArg(&ocl.context, &srcOutputTimes, input, numSamples, 1))
		return NULL;
	if (CL_SUCCESS != CreateWriteBufferArg(&ocl.context, &dstOutputSignal, outputSignalData, numSamples, 1))
		return NULL;

	// Create and build the OpenCL program - imports named cl file.
	if (CL_SUCCESS != ocl.CreateAndBuildProgram(CL_FILENAME))
		return NULL;

	// Create Kernel - kernel name must match kernel name in cl file
	ocl.kernel = clCreateKernel(ocl.program, "PolyEvalOcl", &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clCreateKernel returned %s\n", TranslateOpenCLError(err));
		return NULL;
	}

	// Set OpenCL Kernel Arguments - Order Indicated by Final Argument
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &srcCoefficients, 0))
		return NULL;
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &order, 1))
		return NULL;
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &srcOutputTimes, 2))
		return NULL;
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &dstOutputSignal, 3))
		return NULL;

	// Enqueue Kernel
	size_t localWorkSize[1] = { 16 }; // found via simulation
	size_t globalWorkSize[1] = { numSamples };
	if (CL_SUCCESS != ocl.ExecuteKernel(globalWorkSize, 1, localWorkSize))
		return NULL;

	// Map Host Buffer to Local Data
	cl_float* resultPtr = NULL;
	if (CL_SUCCESS != MapHostBufferToLocal(&ocl.commandQueue, &dstOutputSignal, numSamples, 1, &resultPtr))
	{
		LogError("Error: clEnqueueMapBuffer failed.\n");
		return NULL;
	}
	
	// Copy Data
	cl_float* returnData = tools::CopyMatrix(resultPtr, numSamples, 1);

	// Unmap Host Buffer from Local Data
	if (CL_SUCCESS != UnmapHostBufferFromLocal(&ocl.commandQueue, &dstOutputSignal, resultPtr))
		LogInfo("UnmapHostBufferFromLocal Failed.\n");

	// Free Memory
	_aligned_free(outputSignalData);

	if (CL_SUCCESS != clReleaseMemObject(srcCoefficients))
		LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));
	if (CL_SUCCESS != clReleaseMemObject(srcOutputTimes))
		LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));
	if (CL_SUCCESS != clReleaseMemObject(dstOutputSignal))
		LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));
	
    return returnData;
}

// Test PolyEvalOcl
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
	return 0;
}

// Multiplies two matrices together [A]*[B]
// @param[in] matrixA left matrix
// @param[in] rowsA number of rows of A (M)
// @param[in] colsA number of columns of A (N)
// @param[in] matrixB right colunm vector
// @param[in] rowsB number of rows of B (N)
// @param[in] colsB number of columns of B (L)
// @return output matrix of size MxL
// NOTE: CALLER TAKES OWNERSHIP OF RETURN VALUE
cl_float* MatrixMultiplierOcl(cl_float* matrixA, cl_uint rowsA, cl_uint colsA, cl_float* matrixB, cl_uint rowsB, cl_uint colsB)
{
	cl_int err;
	ocl_args ocl(CL_DEVICE_TYPE_GPU);

	// Create Local Variabels and Allocate Memory
	cl_uint optimizedSize = ((sizeof(cl_float)*rowsA*colsB - 1) / 64 + 1) * 64;
	cl_float* matrixC = (cl_float*)_aligned_malloc(optimizedSize, 4096);
	cl_uint widthA = colsA;
	cl_uint widthB = colsB;
	if(!matrixA || !rowsA || !colsA || !matrixB || !rowsB || !colsB || colsA != rowsB)
	{
		LogError("Error: invalid data input)");
		return NULL;
	}

	// Create OpenCL buffers from host memory for use by Kernel
	cl_mem           srcMatrixA;              // hold the A matrix buffer
	cl_mem           srcMatrixB;              // holds the B matrix buffer
	cl_mem           dstMatrixC;            // hold the C matrix buffer
	if (CL_SUCCESS != CreateReadBufferArg(&ocl.context, &srcMatrixA, matrixA, colsA, rowsA))
		return NULL;
	if (CL_SUCCESS != CreateReadBufferArg(&ocl.context, &srcMatrixB, matrixB, colsB, rowsB))
		return NULL;
	if (CL_SUCCESS != CreateWriteBufferArg(&ocl.context, &dstMatrixC, matrixC, colsB, rowsA))
		return NULL;

	// Create and build the OpenCL program - imports named cl file.
	if (CL_SUCCESS != ocl.CreateAndBuildProgram(CL_FILENAME))
		return NULL;

	// Create Kernel - kernel name must match kernel name in cl file
	ocl.kernel = clCreateKernel(ocl.program, "MatrixMultiplier", &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clCreateKernel returned %s\n", TranslateOpenCLError(err));
		return NULL;
	}

	// Set OpenCL Kernel Arguments - Order Indicated by Final Argument
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &srcMatrixA, 0))
		return NULL;
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &srcMatrixB, 1))
		return NULL;
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &dstMatrixC, 2))
		return NULL;
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &widthA, 3))
		return NULL;
	if (CL_SUCCESS != SetKernelArgument(&ocl.kernel, &widthB, 4))
		return NULL;


	// Enqueue Kernel
	size_t localWorkSize[2] = { 16, 1 }; // found via simulation
	size_t globalWorkSize[2] = { rowsA, colsB };
	if (CL_SUCCESS != ocl.ExecuteKernel(globalWorkSize, 2, localWorkSize))
		return NULL;

	// Map Host Buffer to Local Data
	cl_float* resultPtr = NULL;
	if (CL_SUCCESS != MapHostBufferToLocal(&ocl.commandQueue, &dstMatrixC, colsA, 1, &resultPtr))
	{
		LogError("Error: clEnqueueMapBuffer failed.\n");
		return NULL;
	}
	cl_float* returnData = tools::CopyMatrix(resultPtr, colsA, 1);
	// Unmap Host Buffer from Local Data
	if (CL_SUCCESS != UnmapHostBufferFromLocal(&ocl.commandQueue, &dstMatrixC, resultPtr))
		LogInfo("UnmapHostBufferFromLocal Failed.\n");

	_aligned_free(matrixC);

	if (CL_SUCCESS != clReleaseMemObject(srcMatrixA))
		LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));
	if (CL_SUCCESS != clReleaseMemObject(srcMatrixB))
		LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));
	if (CL_SUCCESS != clReleaseMemObject(dstMatrixC))
		LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));


	return returnData;
}

// Test Matrix MultiplierOcl
int Test_MatrixMultiplierOcl(ResultsStruct* results)
{
	cl_uint rowsA = 3;
	cl_uint colsA = 3;
	cl_float matrixA[] = { 1, 2, 3,
					       4, 5, 6,
						   7, 8, 9 };

	cl_uint rowsB = 3;
	cl_uint colsB = 1;
	cl_float matrixB[3] = { 1,
							2,
							3 };

	cl_float Expected[] = { 14, 32, 50 };

	ProfilerStruct profiler;
	profiler.Start();

	cl_float* Results = MatrixMultiplierOcl(matrixA, rowsA, colsA, matrixB, rowsB, colsB);

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
	return 0;
}

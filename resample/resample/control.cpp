#include "CL/cl.h"
#include "ocl.h"
#include "tools.h"
#include "utils.h"
#include "data.h"
#include "control.h"
#include "profiler.h"
#include "enums.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "windows.h"
#include "shlwapi.h"
#include <fstream>


namespace
{
	const char* FILENAME = "resample.cl";
}

ControlClass::ControlClass()
	: GroupManager("Control")
	, sample_data_(NULL)
	, sample_rate_input_(1000)
	, sample_rate_output_(500)
	, sample_data_input_file_("..\\data\\default.csv")
{
	groups_ = GroupFactory();
}
ControlClass* ControlObject;

std::map<int, ProblemGroup*> ControlClass::GroupFactory()
{
	std::map<int, ProblemGroup*> pgs;

	ProblemGroup* InputControl = GroupManagerInputControlFactory();
	InputControl->problems_[InputControl->problems_.size() + 1] = new Problem(&SetInputDataFile, "Set the file path to read sample data from.");
	InputControl->problems_[InputControl->problems_.size() + 1] = new Problem(&SetSampleRates, "Set the input and output sample rates.");
	InputControl->problems_[InputControl->problems_.size() + 1] = new Problem(&readSampleDataTest, "Read and print sample data.");
	pgs[InputControl->GroupNum()] = InputControl;

	ProblemGroup* projectFuncs = new ProblemGroup(1, "Control");
	projectFuncs->problems_[projectFuncs->problems_.size() + 1] = new Problem(&exCL_Resample, "OpenCL: Apply sixth-order polynomial");
	projectFuncs->problems_[projectFuncs->problems_.size() + 1] = new Problem(&exSeq_Resample, "Sequental: Apply sixth-order polynomial");
	projectFuncs->problems_[projectFuncs->problems_.size() + 1] = new Problem(&exSeq_QRD, "Sequental: QRD");
	pgs[projectFuncs->GroupNum()] = projectFuncs;
	return pgs;
}

// Attempts to read sample input data from provided data file
// Expects to find one amplitude value per line
// @return the number of data points read
int ControlClass::LoadSampleData(bool printPoints)
{
	// With help from:
	//http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
	std::ifstream f(ControlObject->sample_data_input_file_.c_str());
	if (!f.good())
		return -1;
	int pointCount = 0;
	std::filebuf fb;
	std::vector<float> points;
	if (fb.open(ControlObject->sample_data_input_file_, std::ios::in))
	{
		std::istream is(&fb);
		char point[256];
		while (is.getline(point, 256))
			points.push_back(std::stof(point));
		fb.close();
	}
	if (printPoints)
		std::cout << "Read Points: " << std::endl;
	sample_data_ = (float*)malloc(sizeof(float*)*points.size());
	for (size_t i = 0; i < points.size(); ++i)
	{
		sample_data_[i] = points[i];
		if(printPoints)
			std::cout << i << ": " << points[i] << std::endl;
	}

	return points.size();
}
int SetSampleRates(ResultsStruct* results)
{
	std::cout << "Enter INPUT sample rate (currently " << ControlObject->sample_rate_input_ << "): ";
	std::cin >> ControlObject->sample_rate_input_;
	std::cout << "Enter OUTPUT sample rate (currently " << ControlObject->sample_rate_output_ << "): ";
	std::cin >> ControlObject->sample_rate_output_;
	return 0;
}
int SetInputDataFile(ResultsStruct* results)
{
	std::cout << "Enter path to input sample file to (currently " << ControlObject->sample_data_input_file_ << "): ";
	std::string s(ControlObject->sample_data_input_file_);
	std::cin >> s;
	ControlObject->sample_data_input_file_ = s;
	std::ifstream f(ControlObject->sample_data_input_file_.c_str());
	if (!f.good())
	{
		std::cout << "WARNING: File does not exist: " << ControlObject->sample_data_input_file_ << std::endl;
		return -1;
	}

	return 0;
}

// Helper function to call the control class load sample data function
int readSampleDataTest(ResultsStruct* results)
{
	// returns number of loaded
	bool printPoints = true; // for test purposes
	int loadCount = ControlObject->LoadSampleData(printPoints);
	std::cout << "Points Loaded: " << loadCount << std::endl;
	return (loadCount >= 0 ? 0 : -1);
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
	cl_float8        coeffs = {1,2,3,4,5,6,7,0};
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

int exSeq_Resample(ResultsStruct* results)
{
	return 0;
}


////////////////// QR DECOMPOSITION ///////////////
int exCL_QRD(ResultsStruct* results)
{
	return 0;
}

/*
 * Sequential QR decomposition function
 */
void QR(cl_float* R, cl_float* Q, cl_uint arrayWidth, cl_uint arrayHeight)
{
	cl_float a;
	cl_float b;
	cl_float c;
	cl_float s;
	cl_float r;
	cl_float Rnew1[2048];
	cl_float Rnew2[2048];
	cl_float Qnew1[2048];
	cl_float Qnew2[2048];
	for (int j = 0; j < arrayWidth; j++)
	{
		for (int i = arrayHeight - 1; i > j; i--)
		{
			// Calculate Givens rotations
			a = R[arrayWidth * (i - 1) + j];
			b = R[arrayWidth * i + j];
			r = sqrt(a * a + b * b);
			c = a / r;
			s = -b / r;
			// Zero out elements in R matrix
			for (int k = j; k < arrayWidth; k++)
			{
				Rnew1[k] = R[arrayWidth * (i - 1) + k] * c - R[arrayWidth * i + k] * s;
				Rnew2[k] = R[arrayWidth * (i - 1) + k] * s + R[arrayWidth * i + k] * c;
			}
			// Copy new values back to R matrix
			for (int k = j; k < arrayWidth; k++)
			{
				R[arrayWidth * (i - 1) + k] = Rnew1[k];
				R[arrayWidth * i + k] = Rnew2[k];
			}
			// Update Q matrix
			for (int k = 0; k < arrayHeight; k++)
			{
				Qnew1[k] = Q[arrayHeight * (i - 1) + k] * c + Q[arrayHeight * i + k] * s;
				Qnew2[k] = -Q[arrayHeight * (i - 1) + k] * s + Q[arrayHeight * i + k] * c;
			}
			for (int k = 0; k < arrayHeight; k++)
			{
				Q[arrayHeight * (i - 1) + k] = Qnew1[k];
				Q[arrayHeight * i + k] = Qnew2[k];
			}
		}
	}

}

/// @TODO move primary code to helper function
/// @todo write a test function which uses width of 7 with correct 1, t, t^2, t^3 calculations, then
/// have it use a height of the SAMPLE_SIZE
int helper_exSeq_QRD(ResultsStruct* results)
{
	return -1;
}

int exSeq_QRD(ResultsStruct* results)
{
	const cl_uint arrayWidth = 3;
	const cl_uint arrayHeight = 5;
	cl_uint numIter = 10000; // Number of iterations for runtime averaging

							 // allocate working buffers. 
							 // the buffer should be aligned with 4K page and size should fit 64-byte cached line
	cl_uint optimizedSize = ((sizeof(cl_float) * arrayWidth * arrayHeight - 1) / 64 + 1) * 64;
	cl_float* A = (cl_float*)_aligned_malloc(optimizedSize, 4096);

	optimizedSize = ((sizeof(cl_float) * arrayHeight * arrayHeight - 1) / 64 + 1) * 64;
	cl_float* Q = (cl_float*)_aligned_malloc(optimizedSize, 4096);

	cl_float Atmp[] = { 0.8147, 0.0975, 0.1576,
		0.9058, 0.2785, 0.9706,
		0.1270, 0.5469, 0.9572,
		0.9134, 0.9575, 0.4854,
		0.6324, 0.9649, 0.8003 };

	cl_float Qtmp[] = { 1.0,   0,   0,   0,   0,
		                  0, 1.0,   0,   0,   0,
		                  0,   0, 1.0,   0,   0,
		                  0,   0,   0, 1.0,   0,
		                  0,   0,   0,   0, 1.0 };

	if (NULL == A)
	{
		LogError("Error: _aligned_malloc failed to allocate buffers.\n");
		return -1;
	}

	// Initialize A
	for (int i = 0; i < arrayWidth * arrayHeight; i++)
	{
		A[i] = Atmp[i];
	}

	// Initialize Q
	for (int i = 0; i < arrayHeight * arrayHeight; i++)
	{
		Q[i] = Qtmp[i];
	}

	// add
	ProfilerStruct profiler;
	profiler.Start();

	QR(A, Q, arrayWidth, arrayHeight);

	profiler.Stop();
	float runTime = profiler.Log();

	_aligned_free(A);

	results->WindowsRunTime = (double)runTime;
	results->HasWindowsRunTime = true;
	return 0;
}


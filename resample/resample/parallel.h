#pragma once
#include <string>
#include "CL/cl.h"
struct ResultsStruct;

// OpenCL

// Full Reseample Functions
cl_float* ResampleOcl(const std::string& inputFile, size_t inputRate, size_t outputRate, size_t order, cl_float** coeffs, bool verbose);
int Run_ResampleOcl(ResultsStruct* results);

// Polynomial Evaluation
cl_float* PolyEvalOcl(cl_float* coeffs, size_t order, cl_float* input, size_t numSamples);
int Test_PolyEvalOcl(ResultsStruct* results);

// Matrix Multiplication
float* MatrixMultiplierOcl(cl_uint rowsA, cl_uint colsA, float* matrixA, float* matrixB);
int Test_MatrixMultiplierOcl(ResultsStruct* results);

int exCL_Resample(ResultsStruct* results);

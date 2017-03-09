#pragma once
#include "CL/cl.h"
struct ResultsStruct;

// OpenCL

cl_float* PolyEvalOcl(cl_float* coeffs, size_t order, cl_float* input, size_t numSamples);
float* MatrixMultiplierOcl(cl_uint rowsA, cl_uint colsA, float* matrixA, float* matrixB);
int Test_PolyEvalOcl(ResultsStruct* results);
int Test_MatrixMultiplierOcl(ResultsStruct* results);

int exCL_Resample(ResultsStruct* results);

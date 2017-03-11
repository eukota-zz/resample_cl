#pragma once
#include <string>
#include "CL/cl.h"
struct ResultsStruct;

// OpenCL

// Polynomial Evaluation
cl_float* PolyEvalOcl(cl_float* coeffs, size_t order, cl_float* input, size_t numSamples);
int Test_PolyEvalOcl(ResultsStruct* results);

// Matrix Multiplication
float* MatrixMultiplierOcl(cl_uint rowsA, cl_uint colsA, float* matrixA, float* matrixB);
int Test_MatrixMultiplierOcl(ResultsStruct* results);

int exCL_Resample(ResultsStruct* results);

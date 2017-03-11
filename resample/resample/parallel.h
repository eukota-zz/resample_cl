#pragma once
#include <string>
#include "CL/cl.h"
struct ResultsStruct;

// OpenCL

// Polynomial Evaluation
cl_float* PolyEvalOcl(cl_float* coeffs, size_t order, cl_float* input, size_t numSamples);
int Test_PolyEvalOcl(ResultsStruct* results);

// Matrix Multiplication
cl_float* MatrixMultiplierOcl(cl_float* matrixA, cl_uint rowsA, cl_uint colsA, cl_float* matrixB, cl_uint rowsB, cl_uint colsB);
int Test_MatrixMultiplierOcl(ResultsStruct* results);

int exCL_Resample(ResultsStruct* results);

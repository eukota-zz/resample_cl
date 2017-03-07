#pragma once
#include <string>
#include "CL/cl.h"

// Forward Declarations
struct ResultsStruct;

// Resample
cl_float* Resample(const std::string& inputFile, size_t inputRate, size_t outputRate, size_t order, cl_float* coeffs);
int Test_Resample(ResultsStruct* results);

// QR Decomposition
void QR(cl_float* R, cl_float* Q, size_t arrayWidth, size_t arrayHeight);
int Test_QR(ResultsStruct* results);

// Back Substitution
cl_float* BackSub(cl_float* R, cl_float* Qtb, size_t Dim);
int Test_BackSub(ResultsStruct* results);

// Polynomial Evaluation
cl_float* PolyEval(cl_float* coeffs, size_t order, cl_float* input, size_t numSamples);
int Test_PolyEval(ResultsStruct* results);

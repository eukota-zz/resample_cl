#pragma once
#include "CL/cl.h"

// Forward Declarations
struct ResultsStruct;

// Back Substitution
void BackSub(cl_float* R, cl_float* Qtb, size_t Dim, cl_float* Result);
int Test_BackSub(ResultsStruct* results);

// QR Decomposition
void QR(cl_float* R, cl_float* Q, cl_uint arrayWidth, cl_uint arrayHeight);
int Test_QR(ResultsStruct* results);

// Polynomial Evaluation
void PolyEval(cl_float* coeffs, size_t order, cl_float* input, size_t numSamples, cl_float* output);
int Test_PolyEval(ResultsStruct* results);

#pragma once
#include "CL/cl.h"

// Forward Declarations
struct ResultsStruct;

// Back Substitution
void BackSub(cl_float* R, cl_float* Qtb, size_t Dim, cl_float* Result);
int Test_BackSub(ResultsStruct* results);

// Sequential
int exSeq_Resample(ResultsStruct* results);
int exSeq_QRD(ResultsStruct* results);
int exSeq_PolyEval(ResultsStruct* results);

void QR(cl_float* R, cl_float* Q, cl_uint arrayWidth, cl_uint arrayHeight);
#pragma once
#include "CL/cl.h"

// Forward Declarations
struct ResultsStruct;

// Sequential
int exSeq_Resample(ResultsStruct* results);
int exSeq_QRD(ResultsStruct* results);
int exSeq_BackSub(ResultsStruct* results);
int exSeq_PolyEval(ResultsStruct* results);

void QR(cl_float* R, cl_float* Q, cl_uint arrayWidth, cl_uint arrayHeight);
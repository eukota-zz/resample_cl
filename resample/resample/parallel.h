#pragma once
struct ResultsStruct;

// OpenCL
int exCL_Resample(ResultsStruct* results);
int exCL_QRD(ResultsStruct* results);
int Test_PolyEvalOcl(ResultsStruct* results);
int Test_MatrixMultiplierOcl(ResultsStruct* results);

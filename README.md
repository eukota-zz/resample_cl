# Signal Downsampling Using Polynomial LSA and OpenCL

This is a class project for EE590 Applied High-Performance GPU Computing at the University of Washington. The goal is to provide downsampling of a complex IQ data from 4 gigasamples per second (GSPS) to 3.5 GSPS. 

The project has two stages:
   - Complete Sequential Code
   - Add OpenCL Code

## Sequential Code

The sequential code performs a Least Squared Approximation using QR Decomposition and Back Substitution to solve for coefficients. The sequential code is run through its paces in the function *Test_Resample()* within `sequential.cpp`. The general breakdown follows:
   - Read Sample Data
   - Create Time Step Vector
   - Generate A Matrix using Time Step Vector and Polynomial Order (for QR Decomposition)
   - Do QR Decomposition, returning Q and R matrices
   - Multiply Q by Sample Data column vector
   - Use Back Substitution to solve for LSA Coefficients
   - Create Output Time Step Vector
   - Evaluate Polynomial with LSA Coefficients on Output Time Step Vector

It is important to note that we assume a constant input sample rate and sample size. This means that all calculations up through the QR Decomposition only need to happen once before sampling data begins.

## Parallel Code

The parallel code can help in a couple situations. Since startup through QR Decomposition can happen before we get going on downsampling, there is no need to do any of that via parallel.
   - Matrix Multiplication
   - Polynomial Evaluation
   
   

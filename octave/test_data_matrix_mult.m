% Generates an MxM matrix and Mx1 vector and their product for test verification

M = 512;
maxSeed = 10;

matrixA = floor(rand(M,M)*maxSeed);
matrixB = floor(rand(M,1)*maxSeed);
product = matrixA*matrixB;

Aout = matrixA'(:); % outputs row by row as one massive column vector

csvwrite("..\\sessions\\data\\mm_matA.csv", Aout);
csvwrite("..\\sessions\\data\\mm_matB.csv", matrixB(:));
csvwrite("..\\sessions\\data\\mm_matC.csv", product(:));
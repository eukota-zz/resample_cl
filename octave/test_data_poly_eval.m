% Generates MxM matrix and Mx1 vector and their product for test verification

% 7 coefficients for 6th order
order = 6;
coeffs = (rand(order+1,1)*3);
numSamples = 16383;
sampleRate = 1000;

deltaT = 1/sampleRate;
inputTimes = deltaT:deltaT:deltaT*(numSamples+1);

outputValues = polyEval(coeffs, sampleRate, order, numSamples);

csvwrite("..\\sessions\\data\\pe_matA.csv", coeffs(:));
csvwrite("..\\sessions\\data\\pe_matB.csv", inputTimes(:));
csvwrite("..\\sessions\\data\\pe_matC.csv", outputValues(:));
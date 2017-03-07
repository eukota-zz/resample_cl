% Script that calls the polynomial least squares function
% Writes out results to files used by C++ Test Function

clear all
close all
clc

y=csvread("..\\resample\\data\\test_resample_input_signal.csv");
sampleCountIn = numel(y);

% reduce input size since octave chokes on large inputs
sampleCountIn = 200;
y=y(1:sampleCountIn)';

% Input
sampleRateIn = 100;
timeStepIn = 1/sampleRateIn;
endTimeIn = timeStepIn*sampleCountIn;
xIn=timeStepIn:timeStepIn:endTimeIn;
xIn=xIn';

% Output
sampleRateOut = 50;
timeStepOut = 1/sampleRateOut;
endTimeOut = endTimeIn; 
xOut=timeStepOut:timeStepOut:endTimeOut;
sampleCountOut = endTimeOut*sampleRateOut;

% solve for coefficients
k = 7;
coeffs = polyLsqr(xIn, y, k);
order = k;
fit = polyEval(coeffs,sampleRateOut,order,sampleCountOut-1);

% plot figure
figure
plot(xIn,y,'o',xOut,fit,'.-')
legend('data','polynomial fit')

% save data
csvwrite('..\resample\data\test_resample_200_input_signal.csv',y');
csvwrite('..\resample\data\test_resample_200_coeffs.csv',coeffs);
csvwrite('..\resample\data\test_resample_200_output_signal.csv',fit');
  
% Script that calls the polynomial least squares function
% Writes out results to files used by C++ Test Function
clear all
close all
clc

%%% EDIT INPUTS %%%
signalFilePath = "..\\resample\\data\\test_resample_input_signal_FULL.csv"; % 2048 sample points
sampleCountIn = 200; % reduce input size since octave chokes on large inputs
sampleRateIn = 200;
sampleRateOut = 50;
order = 7;
%%%%%%%%%%%%%%%%%%%

signalData=csvread(signalFilePath);
signalData=signalData(1:sampleCountIn)';

% Input
timeStepIn = 1/sampleRateIn;
endTimeIn = timeStepIn*sampleCountIn;
xIn=timeStepIn:timeStepIn:endTimeIn;
xIn=xIn';

% Output
timeStepOut = 1/sampleRateOut;
endTimeOut = endTimeIn; 
xOut=timeStepOut:timeStepOut:endTimeOut;
sampleCountOut = endTimeOut*sampleRateOut;

% solve for coefficients
coeffs = polyLsqr(xIn, signalData, order);
fit = polyEval(coeffs,sampleRateOut,order,sampleCountOut-1);

% plot figure
figure
plot(xIn,signalData,'o',xOut,fit,'.-')
legend('data','polynomial fit')

% save data
saveDataPrefix = '..\\resample\\';
signalDataPath = 'data\\test_resample_input_signal.csv';
csvwrite(strcat(saveDataPrefix, signalDataPath), signalData');
coeffsDataPath = 'data\\test_resample_coeffs.csv';
csvwrite(strcat(saveDataPrefix, coeffsDataPath), coeffs);
outputDataPath = 'data\\test_resample_output_signal.csv';
csvwrite(strcat(saveDataPrefix, outputDataPath), fit');

% write out octave.ini for resample.exe file
cppDataPrefix = '..\\';
octavePrefFile = '..\\resample\\resample\\octave.ini';
writeStringToIniFile(octavePrefFile, 'SignalData', strcat(cppDataPrefix, signalDataPath),1); 
writeStringToIniFile(octavePrefFile, 'CoeffsData', strcat(cppDataPrefix, coeffsDataPath),0); 
writeStringToIniFile(octavePrefFile, 'OutputData', strcat(cppDataPrefix, outputDataPath),0); 
writeIntToIniFile(octavePrefFile, 'SampleInputRate', sampleRateIn,0); 
writeIntToIniFile(octavePrefFile, 'SampleOutputRate', sampleRateOut,0); 
writeIntToIniFile(octavePrefFile, 'PolynomialOrder', order,0); 
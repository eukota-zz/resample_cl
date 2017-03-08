% 
% Step 1: Run polyLsqrTest.m
% Step 2: Run Resample.exe, type "P", then run the Test_Resample function.
% Step 3: Run this file
%

signalDataIn=csvread("..\\resample\\data\\ResampleTest_signalIn.csv");
timeIn=csvread("..\\resample\\data\\ResampleTest_timeIn.csv");
signalDataOut=csvread("..\\resample\\data\\ResampleTest_signalOut.csv");
timeOut=csvread("..\\resample\\data\\ResampleTest_timeOut.csv");


figure
plot(timeIn, signalDataIn, 'o', timeOut, signalDataOut, '.-');
legend('data','cpp polynomial fit')

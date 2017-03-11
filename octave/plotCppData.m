% 
% Step 1: Run polyLsqrTest.m
% Step 2: Run Resample.exe, type "P", then run the Test_Resample function.
% Step 3: Run this file
%

signalDataIn=csvread("..\\resample\\data\\ResampleOutput_SignalIn.csv");
timeIn=csvread("..\\resample\\data\\ResampleOutput_TimeIn.csv");
signalDataOut=csvread("..\\resample\\data\\ResampleOutput_SignalOut.csv");
timeOut=csvread("..\\resample\\data\\ResampleOutput_TimeOut.csv");


figure
plot(timeIn, signalDataIn, 'o', timeOut, signalDataOut, '.-');
legend('data','cpp polynomial fit')

% Script that calls the polynomial least squares function

clear all
close all
clc

%x = [-5,-4,-3,-2,-1,0,1,2,3,4,5];
%y = 5+8*x+3*x.^2+2*x.^3 + 0*rand(1,numel(x));
x = [1,2,3,4];
y = [6,5,7,10];
k = 2;

% todo: (x) function to take sampleRate and numPoints - should return vector of time points
% todo: (y) function generating sample data - should return vector of sample amplitudes

coeffs = polyLsqr(x, y, k);
sampleRate = 1;
order = k;
numPoints = numel(x);
polyEval(coeffs,sampleRate,order,numPoints)

figure
plot(x,y,'o',x,fit,'.-')
legend('data','polynomial fit')

coeffs

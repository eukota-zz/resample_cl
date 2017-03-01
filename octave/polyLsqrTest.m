% Script that calls the polynomial least squares function

clear all
close all
clc

%x = [-5,-4,-3,-2,-1,0,1,2,3,4,5];
%y = 5+8*x+3*x.^2+2*x.^3 + 0*rand(1,numel(x));
x = [1,2,3,4];
y = [6,5,7,10];
k = 2;

a = polyLsqr(x, y, k);

fit = zeros(1,numel(x));
for i=0:k
   fit = fit + a(i+1) .* x.^i;
end

figure
plot(x,y,'o',x,fit,'.-')
legend('data','polynomial fit')

a
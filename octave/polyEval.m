% polynomial evaluation
% a = coefficients vector of size k+1
% r = samples rate (eg: samples per second)
% k = order
% numPoints = number of data points
function [out] = polyEval(a,r,k,numPoints)
  deltaT = 1/r;
  x=deltaT:deltaT:deltaT*(numPoints+1);
  out = zeros(size(x,1));
  for n = 1:k+1
    p = n-1; % exponent power
    out = out + a(n).*x.^p;
  end
  
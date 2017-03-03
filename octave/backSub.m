% R = Upper Triangular Matrix of size NxM
% Qtb = Column Vector of size M
% out = Column Vector of solution to R*out=Qtb
function [out] = backSub(R, Qtb)
    [cols] = size(R,2);
    out = zeros(cols,1);
    % do last value without any subtraction
    out(cols) = Qtb(cols)/R(cols,cols);
    for m=cols-1:-1:1
      % only perform products for known non-zero quantities
      from = m+1;
      to = cols;
      out(m)=(Qtb(m)-R(m,from:to)*out(from:to))/R(m,m);
    end

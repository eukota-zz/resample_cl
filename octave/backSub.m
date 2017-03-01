% R = Upper Triangular Matrix of size NxM
% Qtb = Column Vector of size M
% out = Column Vector of solution to R*out=Qtb
  function [out] = backSub(R, Qtb)
    [rows,cols] = size(R);
    out = zeros(cols,1);
    for m=cols:-1:1
      out(m)=(Qtb(m)-R(m,:)*out)/R(m,m);
    endfor
    
    
    
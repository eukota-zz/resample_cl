% R = Square Matrix of size MxM
% Qtb = Column Vector of size M

  function [out] = backSub(R, Qtb)
    s = size(R,1);
    out = zeros(s,1);
    for m=s:-1:1
      out(m)=(Qtb(m)-R(m,:)*out)/R(m,m);
    endfor
    
    
    
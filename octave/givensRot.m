% Provides the G matrix values for the Givens Rotation of matrix R
% while setting index i,j to zero

function [G] = givensRot(R,i,j)
  a = R(i-1,j);
  b = R(i,j);
  r = sqrt(a^2+b^2);
  c = a/r;
  s = -b/r;
  G = eye(size(R,1));
  G(i-1:i,i-1:i) = [c,s;-s,c];

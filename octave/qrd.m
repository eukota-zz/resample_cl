% Function to compute the QR decomposition of a matrix A

function [Q, R] = qrd(A)
  [m,n] = size(A);
  Q = eye(m,m);
  R = A;
  for j = 1:n
    for i = m:-1:j+1
      G = givensRot(R,i,j);
      R = G' * R;
      Q = Q * G;
    end
  end
end
% Function to compute the QR decomposition of a matrix A

function [Q, R] = qrd(A)

[m,n] = size(A);

Q = eye(size(A,1),size(A,1));
R = A;

for j = 1:n
   indx = size(A,1);
   indy = size(A,1);
   for i = m:-1:j+1
      G = eye(size(A,1));
      %[c,s,r] = givensRot(R(i-1,j),R(i,j));
      a = R(i-1,j);
      b = R(i,j);
      r = sqrt(a^2+b^2);
      c = a/r;
      s = -b/r;
      G(indx-1:indx,indy-1:indy) = [c,s;-s,c];
      indx = indx - 1;
      indy = indy - 1;
      R = G' * R;
      Q = Q * G;
   end
end

end
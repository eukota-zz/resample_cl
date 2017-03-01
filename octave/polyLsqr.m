% Polynomial least squares
% x = time vector
% y = amplitude vector
% k = polynomial order
function a = polyLsqr(x, y, k)
  % Construct x matrix
  X = zeros(numel(x),k+1);
  for i=1:numel(x)
     for j=1:k+1
        if (j == 1) 
           X(i,j) = 1;
        else 
           X(i,j) = x(i)^(j-1);
        end
     end
  end
  a = inv(X' * X) * X' * y';
endfunction
% Worked example for Report
outputFile = '..\\report.txt';
order = 2;
sampleCountIn = 4;
sampleRateIn = 1;
sampleRateOut = 1;

signalData = [9;24;47;78];

% Form A Matrix %
aMatrix = createAMatrix(sampleRateIn,sampleCountIn,order);
writeMatrixToLaTeX(outputFile,'\nA Matrix:\n', aMatrix ,1); 

% Init R Matrix %
rMatrix = aMatrix;
writeMatrixToLaTeX(outputFile, '\nR Matrix Init:\n', rMatrix, 0);

% Iterate for givens:
[m,n] = size(rMatrix);
qMatrix = eye(m,m);
it = 1;
for j = 1:n
  for i = m:-1:j+1
    G = givensRot(rMatrix,i,j);
    rMatrix = G' * rMatrix;
    qMatrix = qMatrix * G;
    writeStringToFile(outputFile, sprintf('\n\\item Iteration %d\n',it), 0);
    writeStringToFile(outputFile, sprintf('\\begin{align*}\n'),0);
    writeMatrixToLaTeX(outputFile, '[R]&=', rMatrix, 0);
    writeStringToFile(outputFile, sprintf('\\\\\n'),0);
    writeMatrixToLaTeX(outputFile, sprintf('[G_%d]&=',it), G, 0);
    writeStringToFile(outputFile, sprintf('\\\\\n'),0);
    writeStringToFile(outputFile, sprintf('\\end{align*}\n'),0);
    it = it+1;
  end
end
writeStringToFile(outputFile, sprintf('\n'),0);

writeStringToFile(outputFile, sprintf('\\begin{align*}\n'),0);
writeMatrixToLaTeX(outputFile, '[Q]&=', qMatrix', 0);
writeStringToFile(outputFile, sprintf('\\\\\n'),0);
writeStringToFile(outputFile, sprintf('\\end{align*}\n'),0);

% Multiply Q Tranpose by Signal Data
Qtb = qMatrix'*signalData;
writeStringToFile(outputFile, sprintf('\\begin{align*}\n'),0);
writeMatrixToLaTeX(outputFile, '[Q]^T&=', Qtb, 0);
writeStringToFile(outputFile, sprintf('\\\\\n'),0);
writeStringToFile(outputFile, sprintf('\\end{align*}\n'),0);

% Back Substitution to Solve for Coefficients
coeffs = backSub(rMatrix, Qtb);
writeMatrixToLaTeX(outputFile, '\\vec{x}&=', coeffs, 0);
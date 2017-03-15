function out = createAMatrix(sampleRate, sampleSize, order)
  
  out=zeros(sampleSize,order+1);
  step = 1/sampleRate;
  for i=1:sampleSize
    for j=0:order
      out(i,j+1)=(step*i)^j;
    end
  end
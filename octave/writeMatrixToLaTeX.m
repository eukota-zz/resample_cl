function writeMatrixToLaTeX(file, annotation, m, overwrite)
  fileID = fopen(file, 'a');
  if(overwrite)
    fclose(fileID);
    fileID = fopen(file, 'w');
  end
  
  [rows,cols]=size(m);  
  fprintf(fileID, annotation);
  fprintf(fileID, '\\begin{bmatrix} \n');
  for i=1:rows
    for j=1:cols
      if(m(i,j) < 0.00001)
        m(i,j) = 0;
      end
      fprintf(fileID, '%d', m(i,j));
      if(j!=cols)
        fprintf(fileID, ' & ');
      end
    end
    if(i!=rows)
      fprintf(fileID, '\\\\');
    end
    fprintf(fileID, '\n');
  end
  fprintf(fileID, '\\end{bmatrix}');
  fclose(fileID);
  end
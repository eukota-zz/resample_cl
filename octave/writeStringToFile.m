function writeStringToFile(file, str, overwrite)
  fileID = fopen(file, 'a');
  if(overwrite)
    fclose(fileID);
    fileID = fopen(file, 'w');
  end
  fprintf(fileID, '%s', str);
  fclose(fileID);
  
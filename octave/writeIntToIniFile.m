function writeIntToIniFile(file, key, value, overwrite)
  fileID = fopen(file, 'a');
  if(overwrite)
    fclose(fileID);
    fileID = fopen(file, 'w');
    fprintf(fileID, '[Octave]\n');
  end
  fprintf(fileID, '%s=%d\n', key, value);
  fclose(fileID);
  end
#include "./io.h"
#include <stdio.h>
#include <stdlib.h>

const char* read_file_io(char* path) {
  if (path == NULL) return NULL;
  FILE* _fp = fopen(path, "r");
  if (_fp == NULL) return NULL;
  
  char* _buffer;
  long _size;
  fseek(_fp, 0L, SEEK_END);
  _size = ftell(_fp);
  rewind(_fp);
  
  _buffer = malloc(_size+1);
  if (_buffer == NULL) return NULL;
  _buffer[_size] = '\0';

  if(fread(_buffer, 1, _size, _fp) != _size) {
    fclose(_fp);
    free(_buffer);
    return NULL;
  }
  fclose(_fp);
  return (const char*)_buffer;
}

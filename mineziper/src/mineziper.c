#include <stdio.h>
#include <stdlib.h>

#include "libmineziper.h"

#define BUF_SIZE 0xfffff

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    printf("NEED ONE FILE TO PROCESS\n");
    return 1;
  }

  FILE* stream = fopen(argv[1], "r");
  if (stream == NULL)
  {
    fprintf(stderr, "Cannot open file for writing\n");
    return 1;
  }

  char buf[BUF_SIZE + 1];
  int read_size = fread(buf, 1, BUF_SIZE, stream);
  buf[BUF_SIZE] = '\0';

  if (scan_zip(buf, read_size))
  {
    printf("[ERROR] Cannot upload file, potential virus detected\n");
  }
  else
  {
    printf("Uploading file...\n");
  }
  return 0;
}
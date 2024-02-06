#include <stdio.h>
#include "libmineziper_zip.h"

#define BUF_SIZE 10000

int main(int argc, char** argv)
{
  FILE* stream = fopen(argv[1], "r");
  if (stream == NULL)
  {
    fprintf(stderr, "Cannot open file for writing\n");
    return -1;
  }

  char buf[BUF_SIZE + 1];
  zip zip;
  raw raw;

  raw.size = fread(buf, BUF_SIZE, 1, stream);
  buf[BUF_SIZE] = '\0';
  raw.buf = buf;

  find_cdh(&raw, &zip);

  return 0;
}
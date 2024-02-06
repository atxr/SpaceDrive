#include <stdio.h>
#include "libmineziper_zip.h"

#define BUF_SIZE 0xfffff

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

  raw.size = fread(buf, 1, BUF_SIZE, stream);
  buf[BUF_SIZE] = '\0';
  raw.buf = buf;

  get_eocd(&raw, &zip);
  get_cdh(&raw, &zip);

  printf(
      "eocd = {\n  nb of entry: %x\n  off cd: %d\n  size cd: 0x%x\n}\n",
      zip.eocd->number_of_entries,
      zip.eocd->off_cdh,
      zip.eocd->size_cdh);

  for (int i = 0; i < zip.eocd->number_of_entries; i++)
  {
    printf(
        "cdh %d = {\n  sig: 0x%x\n  off lfh: 0x%x\n  "
        "comp size: 0x%x\n  uncomp size: 0x%x\n  filename: ",
        i,
        zip.cdh[i]->sig,
        zip.cdh[i]->off_lfh,
        zip.cdh[i]->compressed_size,
        zip.cdh[i]->uncompressed_size);

    for (int j = 0; j < zip.cdh[i]->filename_length; j++)
    {
      printf("%c", ((char*) zip.cdh[i])[sizeof(CDH) + j]);
    }

    printf("\n}\n\n");
  }

  return 0;
}
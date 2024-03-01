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
  int read_size = fread(buf, 1, BUF_SIZE, stream);
  buf[BUF_SIZE] = '\0';

  zip zip = init_zip(buf, read_size);

  printf(
      "eocd = {\n  nb of entry: %x\n  off cd: %d\n  size cd: 0x%x\n}\n",
      zip.entries,
      zip.eocd->off_cdh,
      zip.eocd->size_cdh);

  CDH* cdh = (CDH*) zip.cd;
  for (int i = 0; i < zip.eocd->number_of_entries; i++)
  {
    printf(
        "cdh %d = {\n  sig: 0x%x\n  off lfh: 0x%x\n  "
        "comp size: 0x%x\n  uncomp size: 0x%x\n  filename: ",
        i,
        cdh->sig,
        cdh->off_lfh,
        cdh->compressed_size,
        cdh->uncompressed_size);

    for (int j = 0; j < cdh->filename_length; j++)
    {
      printf("%c", ((char*) cdh)[sizeof(CDH) + j]);
    }

    printf("\n}\n\n");

    cdh = (CDH*) (((char*) cdh) + sizeof(CDH) + cdh->filename_length +
                  cdh->extraf_length + cdh->file_comment_length);
  }

  printf("\n--------------------------------------------\n");

  for (int i = 0; i < zip.entries; i++)
  {
    LFH* lfh = &zip.start[zip.lfh_off[i]];

    printf(
        "lfh %d = {\n  sig: 0x%x\n comp size: 0x%x\n  uncomp size: 0x%x\n  "
        "filename: ",
        i,
        lfh->sig,
        lfh->compressed_size,
        lfh->uncompressed_size);

    for (int j = 0; j < lfh->filename_length; j++)
    {
      printf("%c", ((char*) lfh)[sizeof(LFH) + j]);
    }

    printf("\n}\n\n");
  }

  return 0;
}
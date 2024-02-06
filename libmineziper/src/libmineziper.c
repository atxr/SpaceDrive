#include "libmineziper.h"

bool detect_overlaps(char* filename) {}

int get_uncompressed_size(zip* in)
{
  int size = 0;

  CDH* cdh = in->central_directory;
  while (cdh)
  {
    size += cdh->lfh->size_uncompressed_data;
    cdh = cdh++;
  }

  return size;
}
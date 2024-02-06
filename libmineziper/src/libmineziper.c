#include "libmineziper.h"

bool detect_overlaps(char* filename) {}

int get_uncompressed_size(zip* in)
{
  int size = 0;

  CDH* cdh = in->cd;
  while (cdh)
  {
    size += cdh->lfh->size_uncompressed_data;  // TODO overflow
    cdh++;
  }

  return size;
}
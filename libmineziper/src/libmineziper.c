#include "libmineziper.h"

bool detect_overlaps(char* filename)
{
  return true;
}

int get_uncompressed_size(zip* in)
{
  int size = 0;

  for (int i = 0; i < in->eocd->number_of_entries; i++)
  {
    size += in->cdh[i]->uncompressed_size;  // TODO overflow
  }

  return size;
}

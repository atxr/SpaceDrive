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

int next_token(bitstream* bs, tree t)
{
  // TODO handle if tmin is null elsewhere
  int init = 0;
  if (t.min > 0)
  {
    init = t.min - 1;
  }

  unsigned int base = get_bits(bs, init);

  for (int k = t.min; k <= t.max; k++)
  {
    base += get_bits(bs, 1) << (k - 1);

    for (int i = 0; i < t.size; i++)
    {
      if (t.leaves[i].length == k && t.leaves[i].code == base)
        return t.leaves[i].litteral;
    }
  }

  return -1;
}
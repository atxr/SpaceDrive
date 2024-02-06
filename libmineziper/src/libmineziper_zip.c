#include "libmineziper_zip.h"
#include <string.h>

void find_cdh(raw* raw, zip* out)
{
  if (raw->size < START_CDH_SEARCH)
    return;

  char* se = raw->buf + raw->size - START_CDH_SEARCH;
  while (se > raw->buf)
  {
    if (strcmp(se, CDH_MB) == 0)
    {
      zip->cdh = se;
      break;
    }

    se--;
  }
}
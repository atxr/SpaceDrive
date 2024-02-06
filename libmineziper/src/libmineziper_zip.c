#include "libmineziper_zip.h"
#include <string.h>

void get_eocd(raw* raw, zip* out)
{
  if (raw->size < START_EOCD_SEARCH)
    return;

  char* se = raw->buf + raw->size - START_EOCD_SEARCH;
  while (se > raw->buf)
  {
    if (strcmp(se, EOCD_SIG) == 0)
    {
      out->eocd = se;
      out->cdh = (CDH**) malloc(out->eocd->number_of_entries * sizeof(CDH*));
      break;
    }

    se--;
  }
}

void get_cdh(raw* raw, zip* out)
{
  if (out->eocd == 0 || out->eocd->off_cdh == 0)
  {
    printf("<get_cdh> error: No EOCD found.\n");
    exit(-1);
  }

  out->cd = raw->buf + out->eocd->off_cdh;

  CDH* cdh = out->cd;
  for (int i = 0; i < out->eocd->number_of_entries; i++)
  {
    out->cdh[i] = cdh;
    cdh = (CDH*) (((char*) cdh) + sizeof(CDH) + cdh->filename_length +
                  cdh->extra_field_length + cdh->file_comment_length);
  }
}
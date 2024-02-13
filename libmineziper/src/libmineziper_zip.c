#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libmineziper_zip.h"

void get_eocd(raw* raw, zip* out)
{
  if (raw->size < START_EOCD_SEARCH)
    return;

  char* se = raw->buf + raw->size - START_EOCD_SEARCH;
  while (se > raw->buf)
  {
    if (strcmp(se, EOCD_SIG) == 0)
    {
      out->eocd = (EOCD*) se;
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

  CDH* cdh = (CDH*) out->cd;
  for (int i = 0; i < out->eocd->number_of_entries; i++)
  {
    out->cdh[i] = cdh;
    cdh = (CDH*) (((char*) cdh) + sizeof(CDH) + cdh->filename_length +
                  cdh->extra_field_length + cdh->file_comment_length);
  }
}

void decode_huffman_tree(char* encoded, HT* out)
{
  unsigned char size = *encoded;
  if (size == 0)
  {
    printf("ERROR WTF");
    // TODO
    return;
  }

  if (size > NUM_OF_CODE)
  {
    printf("To many symbols");
    // TODO
    return;
  }

  HT tree = malloc(sizeof(HN) * size);
  encoded++;

  char code = 0;
  char len_diff = 0;

  for (int i = 0; i < size; i++)
  {
    tree[i].symbol = SYMBOLS[i];
    tree[i].code = code;
    tree[i].len = *encoded;

    if (i + 1 < size)
    {
      // TODO WHAT IS LENDIFF IS BIG?
      len_diff = encoded[1] - encoded[0];
      code = (code + 1) << len_diff;
      encoded++;
    }
  }

  *out = tree;
}

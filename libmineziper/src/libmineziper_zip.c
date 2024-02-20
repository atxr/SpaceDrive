#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libmineziper_huffman_tree.h"
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
      out->lfh = (LFH**) malloc(out->eocd->number_of_entries * sizeof(LFH*));
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
    out->lfh[i] = (LFH*) (raw->buf + cdh->off_lfh);

    cdh = (CDH*) (((char*) cdh) + sizeof(CDH) + cdh->filename_length +
                  cdh->extra_field_length + cdh->file_comment_length);
  }
}

char* get_encoded_block(zip* in, int n)
{
  return (char*) (in->lfh[n]) + sizeof(LFH) + in->lfh[n]->filename_length +
         in->lfh[n]->extra_field_length;
}

char* decode_type1_block(bitstream* bs, int uncompressed_size, char* decoded_data)
{
  tree tr = build_default_tree();
  tree tr_dist = build_default_dist_tree();

  int i = 0, token;
  while (i < uncompressed_size && (token = next_token(bs, tr)) != END_OF_BLOCK)
  {
    if (token < END_OF_BLOCK)
    {
      printf("token[%d]: 0x%x\n", i, token);
      decoded_data[i++] = token;
    }

    else
    {
      int length = decode_length_token(bs, token);

      if ((token = next_token(bs, tr_dist)) == END_OF_BLOCK)
      {
        printf("[ERROR] Got EndOfBlock when decoding distance token\n");
        exit(1);
      }

      int distance = decode_distance_token(bs, token);

      printf("token[%d]: token[-%d] with length %d\n", i, distance, length);

      for (int j = 0; j < length; j++)
      {
        decoded_data[i] = decoded_data[i - distance];
        i++;
      }
    }
  }
}

short decode_length_token(bitstream* bs, int token)
{
  token -= END_OF_BLOCK + 1;
  int extra = get_bits(bs, extra_bits_length_codes[token]);
  return length_codes[token] + extra;
}

int decode_distance_token(bitstream* bs, int token)
{
  int extra = get_bits(bs, extra_bits_distance_codes[token]);
  return distance_codes[token] + extra;
}
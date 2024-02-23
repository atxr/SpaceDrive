#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libmineziper_huffman_tree.h"
#include "libmineziper_zip.h"

zip init_zip(char* data, int size)
{
  zip z = {.start = data, .size = size};

  get_eocd(&z);
  get_cdh(&z);

  return z;
}

void get_eocd(zip* z)
{
  if (z->size < START_EOCD_SEARCH)
    return;

  char* se = &z->start[z->size - START_EOCD_SEARCH];
  while (se > z->start)
  {
    if (strcmp(se, EOCD_SIG) == 0)
    {
      z->eocd = (EOCD*) se;
      z->entries = z->eocd->number_of_entries;

      z->lfh_off = malloc(z->entries * sizeof(int));
      z->cdh_filename_length = malloc(z->entries * sizeof(int));

      if (!z->lfh_off || !z->cdh_filename_length)
      {
        printf(
            "[ERROR] Failed to allocate CDH/LFH buffer for %d entries\n",
            z->entries);
        exit(1);
      }

      break;
    }

    se--;
  }
}

void get_cdh(zip* z)
{
  if (z->eocd == 0 || z->eocd->off_cdh == 0)
  {
    fprintf(stderr, "[ERROR]: No EOCD found when fetching CDH.\n");
    exit(-1);
  }

  z->cd = z->start + z->eocd->off_cdh;

  CDH* cdh = (CDH*) z->cd;
  for (int i = 0; i < z->eocd->number_of_entries; i++)
  {
    z->lfh_off[i] = cdh->off_lfh;
    z->cdh_filename_length[i] = cdh->filename_length;

    cdh = (CDH*) (((char*) cdh) + sizeof(CDH) + cdh->filename_length +
                  cdh->extraf_length + cdh->file_comment_length);
  }
}

char* decode_type1_block_vuln(bitstream* bs, char* decoded_data)
{
  tree tr = build_default_tree();
  tree tr_dist = build_default_dist_tree();

  int i = 0, token;
  while ((token = next_token(bs, tr)) != END_OF_BLOCK)
  {
    if (token < END_OF_BLOCK)
    {
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

      for (int j = 0; j < length; j++)
      {
        decoded_data[i] = decoded_data[i - distance];
        i++;
      }
    }
  }

  free_tree(tr);
  free_tree(tr_dist);
}

char* decode_type1_block_v2(
    bitstream* bs,
    int uncompressed_size,
    char* decoded_data)
{
  tree tr = build_default_tree();
  tree tr_dist = build_default_dist_tree();

  int i = 0, token;
  while (i < uncompressed_size && (token = next_token(bs, tr)) != END_OF_BLOCK)
  {
    if (token < END_OF_BLOCK)
    {
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

      for (int j = 0; j < length; j++)
      {
        decoded_data[i] = decoded_data[i - distance];
        i++;
      }
    }
  }
}

int get_number_bit_length_code(DHCH* dhch)
{
  return (dhch->bit_length_code & 0b1111) + 4;
}

int get_number_dist_code(DHCH* dhch)
{
  return (dhch->dist_codes & 0b11111) + 1;
}

int get_number_litteral_code(DHCH* dhch)
{
  return (dhch->literal_codes & 0b11111) + 257;
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

void free_zip(zip z)
{
  free(z.cdh_filename_length);
  free(z.lfh_off);
}
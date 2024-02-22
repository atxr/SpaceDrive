#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libmineziper.h"

int get_uncompressed_size(zip* in)
{
  int size = 0;

  for (int i = 0; i < in->entries; i++)
  {
    LFH* lfh = &in->start[in->lfh_off[i]];
    size += lfh->uncompressed_size;
  }

  return size;
}

bool scan_zip(char* zip_data, int zip_size)
{
  zip zip = init_zip(zip_data, zip_size);

  for (int i = 0; i < zip.entries; i++)
  {
    LFH* lfh = zip.start + zip.lfh_off[i];

    // Verify CDH/LFH parsed sizes to avoid undefined behavior
    if (lfh->filename_length != zip.cdh_filename_length[i])
      {
        printf("[ERROR] Mismatch in CDH/LFH filename lengths.\n");
        return true;
      }

    char* decoded_data = "";
    if (lfh->compression_method == DEFLATE)
    {
      int lfh_length = sizeof(LFH) + lfh->filename_length + lfh->extraf_length;
      char* encoded_block = &((char*) lfh)[lfh_length];

      bitstream bs = init_bitstream(encoded_block, lfh->compressed_size, 0);
      ISH deflate_header = {.raw = get_bits(&bs, 3)};

      // Stored block
      if (deflate_header.block_type == 0)
      {
        align_to_next_byte(&bs);
        short block_size = get_bits(&bs, 16);
        short inv_block_size = get_bits(&bs, 16);
        assert(block_size == ~inv_block_size);

        decoded_data = malloc(block_size);

        memcpy(decoded_data, &bs.data[bs.current_data_offset], block_size);
      }

      // Fixed Huffman Codes
      else if (deflate_header.block_type == 1)
      {
        printf("[FILE %d] Scanning 1 block...\n", i);
        decoded_data = malloc(lfh->uncompressed_size);

        decode_type1_block_vuln(&bs, decoded_data);
      }

      // Dynamic Huffman Codes
      else if (deflate_header.block_type == 2)
      {
        fprintf(
            stderr,
            "[FILE %d] Dynamic Huffman codes block type not supported\n",
            i);
      }

      // Invalid type
      else
      {
        fprintf(stderr, "[FILE %d] Error in compressed data\n", i);
      }
    }
    else
    {
      fprintf(stderr, "Unknown decompression algorithm. Skipping...\n");
    }

    // Test the decoded data
    if (strcmp("VIRUS", decoded_data) == 0)
    {
      printf("-> VIRUS FOUND\n");
      return true;
    }
    else
    {
      printf("-> OK\n\n");
    }
  }

  return false;
}

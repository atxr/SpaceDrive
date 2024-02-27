#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libmineziper.h"

int get_uncompressed_size(zip zip)
{
  unsigned int size = 0;

  for (int i = 0; i < zip.entries; i++)
  {
    unsigned int file_size =
        ((LFH*) zip.start + zip.lfh_off[i])->uncompressed_size;

    if (size > MAX_INT / 2 && file_size > MAX_INT / 2)
    {
      return MAX_INT;
    }
    size += file_size;
  }

  return size;
}

bool detect_overlaps(zip zip)
{
  // TODO sort only deals with char*
  int* lfh_order = sort_int(zip.lfh_off, zip.entries);
  bool overlap = false;

  for (int i = 0; i < zip.entries - 1; i++)
  {
    LFH* lfh = (LFH*) zip.start + zip.lfh_off[i];
    unsigned int lf_size = sizeof(LFH) + lfh->filename_length +
                           lfh->extraf_length + lfh->compressed_size;

    if (lf_size > zip.lfh_off[i + 1] - zip.lfh_off[i])
    {
      overlap = true;
      break;
    }
  }

  free(lfh_order);
  return overlap;
}

bool scan_decoded_files(zip zip)
{
  for (int i = 0; i < zip.entries; i++)
  {
    LFH* lfh = (LFH*) (zip.start + zip.lfh_off[i]);

    // Verify CDH/LFH parsed sizes to avoid undefined behavior
    if (lfh->filename_length != zip.cdh_filename_length[i])
    {
      fprintf(
          stderr,
          "[ERROR] Mismatch in CDH/LFH filename lengths. Local file might be "
          "malformed.\nSkipping file...\n");
      continue;
    }

    for (int k = 0; k < blocklist_size; k++)
    {
      char* filename = zip.start + zip.lfh_off[i] + sizeof(LFH);
      if (strcmp(blocklist[k], filename) == 0)
      {
        fprintf(stderr, "[ERROR] Forbidden filename found in zip archive.\n");
        return true;
      }
    }

    data* decoded = malloc(sizeof(data));
    decoded->buffer = decoded->clean = decoded->size = 0;

    if (lfh->compression_method == COMP_NONE)
    {
      printf("[FILE %d] Scanning stored data...\n", i);
      int lfh_length = sizeof(LFH) + lfh->filename_length + lfh->extraf_length;
      char* block = &((char*) lfh)[lfh_length];
      decoded->size = lfh->uncompressed_size;

      decoded->buffer = (char*) malloc(decoded->size);
      if (!decoded->buffer)
      {
        fprintf(
            stderr,
            "[FILE %d] Failed to allocate buffer. Skipping this block.\n",
            i);
        continue;
      }
      decoded->clean = &free;

      memcpy(decoded->buffer, block, decoded->size);
    }

    else if (lfh->compression_method == COMP_DEFLATE)
    {
      printf("[FILE %d] Scanning first block of DEFLATED data...\n", i);

      int lfh_length = sizeof(LFH) + lfh->filename_length + lfh->extraf_length;
      char* encoded_block = &((char*) lfh)[lfh_length];

      bitstream bs = init_bitstream(encoded_block, lfh->compressed_size, 0);
      ISH deflate_header = {.raw = get_bits(&bs, 3)};

      // Stored block
      if (deflate_header.block_type == 0)
      {
        align_to_next_byte(&bs);
        decoded->size = get_bits(&bs, 16);
        short inv_block_size = get_bits(&bs, 16);
        if ((short) decoded->size != ~inv_block_size)
        {
          fprintf(stderr, "-> Sizes mismatch in block type 0.\n");
          return true;
        }

        decoded->buffer = (char*) malloc(decoded->size);
        if (!decoded->buffer)
        {
          fprintf(
              stderr, "-> Failed to allocate buffer. Skipping this block.\n");
          continue;
        }
        decoded->clean = &free;

        memcpy(
            decoded->buffer, &bs.data[bs.current_data_offset], decoded->size);
      }

      // Fixed Huffman Codes
      else if (deflate_header.block_type == 1)
      {
        decoded->size = lfh->uncompressed_size;

        decoded->buffer = (char*) malloc(decoded->size);
        if (!decoded->buffer)
        {
          fprintf(
              stderr, "-> Failed to allocate buffer. Skipping this block.\n");
          continue;
        }
        decoded->clean = &free;

        decode_type1_block_vuln(&bs, decoded->buffer);
      }

      // Dynamic Huffman Codes
      else if (deflate_header.block_type == 2)
      {
        fprintf(stderr, "-> Dynamic Huffman codes block type not supported\n");
      }

      // Invalid type
      else
      {
        fprintf(stderr, "-> Error in compressed data\n");
      }
    }
    else
    {
      fprintf(
          stderr,
          "[FILE %d] Unknown decompression algorithm. Skipping...\n",
          i);
    }

    if (decoded->size)
    {
      // Test magic bytes of the decoded data
      unsigned int cmp_size;
      for (int s = 0; s < sigs_size; s++)
      {
        cmp_size = (decoded->size < strlen(sigs[s])) ? decoded->size
                                                     : strlen(sigs[s]);

        if (memcmp(sigs[s], decoded->buffer, cmp_size) == 0)
        {
          return true;
        }
      }
    }

    // If allocated with malloc, free the buffer
    if (decoded->clean)
    {
      decoded->clean(decoded->buffer);
    }

    free(decoded);
  }

  return false;
}

bool scan_zip(char* zip_data, int zip_size)
{
  zip zip = init_zip(zip_data, zip_size);

  if (get_uncompressed_size(zip) > MAX_UNCOMPRESSED_SIZE)
  {
    fprintf(stderr, "Uncompressed size too big. Could be a zip bomb.\n");
    return true;
  }

  if (detect_overlaps(zip))
  {
    fprintf(
        stderr, "Potential overlaps found in zip file. Could be a zip bomb.\n");
    return true;
  }

  if (scan_decoded_files(zip))
  {
    fprintf(
        stderr, "Decompressed files looks malicious. Could contain a virus.\n");
    return true;
  }

  return false;
}

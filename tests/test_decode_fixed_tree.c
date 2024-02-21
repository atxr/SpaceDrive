#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "libmineziper.h"
#include "libmineziper_huffman_tree.h"
#include "libmineziper_zip.h"

#define BUF_SIZE 0xfffff

void main()
{
  char* filename = "x.zip";

  FILE* stream = fopen(filename, "r");
  if (stream == NULL)
  {
    fprintf(stderr, "Cannot open file for writing\n");
    return;
  }

  char buf[BUF_SIZE + 1];
  int read_size = fread(buf, 1, BUF_SIZE, stream);
  buf[BUF_SIZE] = '\0';

  zip zip;
  get_eocd(buf, read_size, &zip);
  get_cdh(buf, &zip);

  for (int k = 0; k < zip.eocd->number_of_entries; k++)
  {
    printf("BLOC %d:\n", k);

    unsigned int uncompressed_size = zip.lfh[k]->uncompressed_size;
    printf("UNCOMPRESSED SIZE: 0x%x\n", uncompressed_size);

    if (zip.lfh[k]->compressed_size == 0)
    {
      printf("Empty bloc\n");
    }
    else
    {
      if (zip.cdh[k]->compression_method == DEFLATE)
      {
        char* data = get_encoded_block(&zip, k);

        bitstream bs = init_bitstream(data, 0 /*TODO WWEUWIEUWIEUWI*/, 0);

        ISH deflate_header = {.raw = get_bits(&bs, 3)};
        printf(
            "DEFLATE BLOC:\nLast bloc bit: %d\nBloc type: %d\n",
            deflate_header.last_block,
            deflate_header.block_type);

        if (deflate_header.block_type == 1)
        {
          char* decoded_data = malloc(uncompressed_size);

          decode_type1_block(&bs, uncompressed_size, decoded_data);

          FILE* tmp_file = fopen("/tmp/test.txt", "w");
          fwrite(decoded_data, 1, uncompressed_size, tmp_file);
          fclose(tmp_file);

          free(decoded_data);
        }

        else if (deflate_header.block_type == 2)
        {
          // handle_type2(data);
        }
      }

      else
      {
        printf("Compression method not recognized\n");
      }
    }

    printf("\n");
  }
}

/*
void handle_type2(char* data)
{
  DHCH* dynamic_huffman_code_header = data;
  data += sizeof(DHCH);

  printf(
      "Dynamic header:\n# of bit length codes: %d\n# of dist codes: "
      "%d\n# of litteral codes: %d\n",
      get_number_bit_length_code(dynamic_huffman_code_header),
      get_number_dist_code(dynamic_huffman_code_header),
      get_number_litteral_code(dynamic_huffman_code_header));

#define CODE_LENGTH 19
  // read 19 codes defining bit lengths
  HN code_length[CODE_LENGTH];
  const unsigned char code_length_order[CODE_LENGTH] = {
      16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

  for (size_t i = 0; i < CODE_LENGTH; i++)
  {
    memcpy(&code_length[code_length_order[i]].len, data, 3);
    data += 3;
  }

  for (size_t i = 0; i < CODE_LENGTH; i++)
  {
    printf("For length %d: %d\n", i, code_length[i].len);
  }
}
*/
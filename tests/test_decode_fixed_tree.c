#include <stdio.h>
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
  zip zip;
  raw raw;

  raw.size = fread(buf, 1, BUF_SIZE, stream);
  buf[BUF_SIZE] = '\0';
  raw.buf = buf;
  raw.stream = buf;

  get_eocd(&raw, &zip);
  get_cdh(&raw, &zip);

  for (int k = 0; k < zip.eocd->number_of_entries; k++)
  {
    printf("BLOC %d:\n", k);

    unsigned int real_size = zip.lfh[k]->uncompressed_size;

    if (zip.lfh[k]->compressed_size == 0)
    {
      printf("Empty bloc\n");
    }
    else
    {
      if (zip.cdh[k]->compression_method == DEFLATE)
      {
        char* data = get_encoded_data(&zip, k);

        bitstream bs = init_bitstream(data, 0 /*TODO WWEUWIEUWIEUWI*/, 0);

        ISH deflate_header = {.raw = get_bits(&bs, 3)};
        printf(
            "DEFLATE BLOC:\nLast bloc bit: %d\nBloc type: %d\n",
            deflate_header.last_block,
            deflate_header.block_type);

        if (deflate_header.block_type == 1)
        {
          tree tr = build_default_tree();
          tree tr_dist = build_default_dist_tree();

          char* decoded_data = malloc(real_size);
          int i = 0;
          int token;
          while (i < real_size && (token = next_token(&bs, tr)) != END_OF_BLOCK)
          {
            if (token < END_OF_BLOCK)
            {
              printf("token[%d]: 0x%x\n", i, token);
              decoded_data[i++] = token;
            }

            else
            {
              int length = decode_length_token(&bs, token);

              if ((token = next_token(&bs, tr_dist)) == END_OF_BLOCK)
              {
                printf("[ERROR] Got EndOfBlock when decoding distance token\n");
                exit(1);
              }

              int distance = decode_distance_token(&bs, token);

              printf(
                  "token[%d]: token[-%d] with length %d\n",
                  i,
                  distance,
                  length);

              for (int j = 0; j < length; j++)
              {
                decoded_data[i] = decoded_data[i - distance];
                i++;
              }
            }
          }

          FILE* tmp_file = fopen("/tmp/test.txt", "w");
          fwrite(decoded_data, 1, i, tmp_file);
          fclose(tmp_file);
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
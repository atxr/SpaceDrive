#ifndef LIBMINEZIPER_ZIP_H
#define LIBMINEZIPER_ZIP_H

#include "libmineziper_bitstream.h"

#pragma pack(1)

#define START_EOCD_SEARCH 22
#define EOCD_SIG          "PK\05\06"
#define LFH_SIG           "PK\03\04"
#define CDH_SIG           "PK\01\02"
#define DEFLATE           8

#define END_OF_BLOCK 256

typedef struct LFH
{
  int sig;  // LFH_SIG "PK\03\04"
  short version;
  short f;
  short compression_method;
  short last_mod_time;
  short last_mod_date;
  int crc32;
  int compressed_size;
  int uncompressed_size;
  short filename_length;
  short extraf_length;
} LFH;

typedef struct CDH
{
  int sig;  // CDH_SIG "PK\01\02"
  short v1;
  short v2;
  short f;
  short compression_method;
  short last_mod_time;
  short last_mod_date;
  int crc32;
  int compressed_size;
  int uncompressed_size;
  short filename_length;
  short extraf_length;
  short file_comment_length;
  short disk_number_start;
  short internal_file_attributes;
  int external_file_attributes;
  int off_lfh;
} CDH;

typedef struct EOCD
{
  int sig;  // CDH_SIG "PK\05\06"
  char d[6];
  short number_of_entries;
  int size_cdh;
  int off_cdh;
} EOCD;

// Input stream header for DEFLATE
typedef struct ISH
{
  union
  {
    int raw;
    struct
    {
  unsigned last_block : 1;
  unsigned block_type : 2;
    };
  };
} ISH;

// Dynamic Huffman Code header for DEFLATE
typedef struct DHCH
{
  unsigned literal_codes : 5;
  unsigned dist_codes : 5;
  unsigned bit_length_code : 4;
} DHCH;

typedef struct zip
{
  // compression type
  char* cd;
  CDH** cdh;
  LFH** lfh;
  unsigned int* lfh_off;
  unsigned int entries;
  EOCD* eocd;
} zip;

void get_eocd(char* data, int size, zip* out);
void get_cdh(char* data, zip* out);
char* get_encoded_block(zip* in, int n);
void parse_zip(char* filename, zip* out);
void deflate(zip* in);


short decode_length_token(bitstream* bs, int token);
int decode_distance_token(bitstream* bs, int token);

char* decode_type1_block_vuln(bitstream* bs, char* decoded_data);
char* decode_type1_block_v2(
    bitstream* bs,
    int uncompressed_size,
    char* decoded_data);

static const short length_codes[] = {
    3,  4,  5,  6,  7,  8,  9,  10, 11,  13,  15,  17,  19,  23, 27,
    31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};

static const char extra_bits_length_codes[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                                               1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
                                               4, 4, 4, 4, 5, 5, 5, 5, 0};

static const int distance_codes[] = {
    1,    2,    3,    4,    5,    7,    9,    13,    17,    25,
    33,   49,   65,   97,   129,  193,  257,  385,   513,   769,
    1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};

static const char extra_bits_distance_codes[] = {
    0, 0, 0, 0, 1, 1, 2, 2,  3,  3,  4,  4,  5,  5,  6,
    6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

#endif
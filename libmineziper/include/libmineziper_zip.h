#ifndef LIBMINEZIPER_ZIP_H
#define LIBMINEZIPER_ZIP_H

#pragma pack(1)

#define START_EOCD_SEARCH 22
#define EOCD_SIG          "PK\05\06"
#define LFH_SIG           "PK\03\04"
#define CDH_SIG           "PK\01\02"
#define DEFLATE           8

#define NUM_OF_CODE 26 * 2 + 10
const char* SYMBOLS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

typedef struct raw
{
  char* buf;
  char* stream;
  int size;
} raw;

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
  short extra_field_length;
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
  short extra_field_length;
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
  unsigned last_block : 1;
  unsigned block_type : 2;
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
  EOCD* eocd;
} zip;

// Huffman Node and Table
typedef struct HN
{
  unsigned char symbol;
  unsigned char code;
  unsigned char len;
} HN;

typedef struct HT
{
  unsigned char size;
  HN* nodes;
} HT;

void get_eocd(raw* raw, zip* out);
void get_cdh(raw* raw, zip* out);
char* get_encoded_data(zip* in, int n);
void parse_zip(char* filename, zip* out);
void deflate(zip* in);

#endif
#ifndef LIBMINEZIPER_ZIP_H
#define LIBMINEZIPER_ZIP_H

#define START_CDH_SEARCH 22
#define CDH_MB           "PK\05\06"
#define LFH_MB           "PK\03\04"

typedef struct raw
{
  char* buf;
  int size;
} raw;

typedef struct CDH
{
  char mb[4];       // CDH_MB "PK\05\06"
  void* crc32;      // TODO TYPE?
  LFH* lfh;
  void* filename;   // TODO TYPE?
} CDH;

typedef struct LFH
{
  char mb[4];       // CDH_MB "PK\03\04"
  void* crc32;  // TODO TYPE?
  LFH* lfh;
  int size_compressed_data;
  int size_uncompressed_data;
  void* filename;  // TODO TYPE?
  void* data;
} LFH;

typedef struct zip
{
  // compression type
  CDH* central_directory;
} zip;

void find_cdh(raw* raw, zip* out);
void parse_zip(char* filename, zip* out);

#endif
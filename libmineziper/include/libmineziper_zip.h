#ifndef LIBMINEZIPER_ZIP_H
#define LIBMINEZIPER_ZIP_H

typedef struct CDH
{
  void* crc32;  // TODO TYPE?
  LFH* lfh;
  void* filename;  // TODO TYPE?
} CDH;

typedef struct LFH
{
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

void parse_zip(char* filename, zip* out);

#endif
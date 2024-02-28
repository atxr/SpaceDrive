#ifndef LIBMINEZIPER_H
#define LIBMINEZIPER_H

#include <stdbool.h>
#include "libmineziper_bitstream.h"
#include "libmineziper_huffman_tree.h"
#include "libmineziper_zip.h"

#define MAX_UNCOMPRESSED_SIZE 0x10000000
#define MAX_INT               0xffffffff

typedef struct data
{
  unsigned int size;
  void (*clean)(void*);
  char* buffer;
} data;

int get_uncompressed_size(zip zip);
bool detect_overlaps(zip zip);
bool scan_decoded_files(zip zip);
bool scan_zip(char* zip_data, int zip_size);

static const char* sigs[] = {
    "\x7f"
    "ELF",
    "MZ"};
static const int sigs_size = sizeof(sigs) / sizeof(char*);

#endif
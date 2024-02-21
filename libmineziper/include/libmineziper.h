#ifndef LIBMINEZIPER_H
#define LIBMINEZIPER_H

#include <stdbool.h>
#include "libmineziper_bitstream.h"
#include "libmineziper_huffman_tree.h"
#include "libmineziper_zip.h"

bool detect_overlaps(char* filename);
int get_uncompressed_size(zip* in);
bool scan_zip(char* zip_data, int zip_size);

#endif
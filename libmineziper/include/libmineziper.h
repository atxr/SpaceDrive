#ifndef LIBMINEZIPER_H
#define LIBMINEZIPER_H

#include "libmineziper_zip.h"
#include <stdbool.h>

bool detect_overlaps(char* filename);
int get_uncompressed_size(zip* in);

#endif
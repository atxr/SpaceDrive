#ifndef LIBMINEZIPER_HUFFMAN_TREE_H
#define LIBMINEZIPER_HUFFMAN_TREE_H

#include "libmineziper_bitstream.h"

typedef struct leaf
{
  int code;
  short litteral;
  char length;
} leaf;

typedef struct tree
{
  leaf* leaves;
  unsigned int size;
  unsigned int min, max;
} tree;

unsigned int* sort(unsigned char* ints, int size);
void print_huffman_tree(tree t);
tree build_tree(char* bit_lengths, int size);
tree build_default_tree();

#endif
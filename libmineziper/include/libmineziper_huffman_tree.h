#ifndef LIBMINEZIPER_HUFFMAN_TREE_H
#define LIBMINEZIPER_HUFFMAN_TREE_H

#include "libmineziper_bitstream.h"

#define DEFAULT_TREE_SIZE      288
#define DEFAULT_DIST_TREE_SIZE 32

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
tree build_default_dist_tree();

int next_token(bitstream* bs, tree t);

#endif
#include "libmineziper_huffman_tree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tree build_tree(char* bit_lenghts, int size)
{
  unsigned int* order_codes = sort(bit_lenghts, size);

  tree out;
  out.size = size;
  out.min = bit_lenghts[order_codes[0]];
  out.max = bit_lenghts[order_codes[size - 1]];
  out.leaves = malloc(sizeof(leaf) * size);

  int code = 0;
  int code_incr = 0;
  int last_bit_length = 0;
  int i = 0;
  int len_bit, j;

  while (i < size)
  {
    j = order_codes[i];
    len_bit = bit_lenghts[j];

    code = code + code_incr;
    if (len_bit != last_bit_length)
    {
      last_bit_length = len_bit;
      code_incr = 1 << (16 - last_bit_length);
    }

    out.leaves[j].code = reverse(code, 16);
    out.leaves[j].length = len_bit;
    out.leaves[j].litteral = j;
    i++;
  }

  free(order_codes);
  return out;
}

tree build_default_tree()
{
  char bit_lengths[DEFAULT_TREE_SIZE];
  for (int i = 0; i < DEFAULT_TREE_SIZE; i++)
  {
    if (i < 144 || i >= 280)
      bit_lengths[i] = 8;

    else if (i >= 144 && i < 256)
      bit_lengths[i] = 9;

    else
      bit_lengths[i] = 7;
  }

  return build_tree(bit_lengths, DEFAULT_TREE_SIZE);
}

tree build_default_dist_tree()
{
  char bit_lengths[DEFAULT_DIST_TREE_SIZE];
  for (int i = 0; i < DEFAULT_DIST_TREE_SIZE; i++)
  {
    bit_lengths[i] = 5;
  }

  return build_tree(bit_lengths, DEFAULT_DIST_TREE_SIZE);
}

unsigned int* sort(unsigned char* ints, int size)
{
  unsigned char MAX = 0xff;

  unsigned int* out = malloc(sizeof(int) * size);
  unsigned char* buf = malloc(size);
  memcpy(buf, ints, sizeof(char) * size);

  for (int k = 0; k < size; k++)
  {
    int min = 0;
    for (int i = 0; i < size; i++)
    {
      if (buf[min] == MAX || buf[i] < ints[min])
      {
        min = i;
      }
    }
    buf[min] = MAX;
    out[k] = min;
  }

  free(buf);
  return out;
}

void print_huffman_tree(tree t)
{
  for (int i = 0; i < t.size; i++)
  {
    printf("Symbol 0x%x = Code ", t.leaves[i].litteral);
    print_bits(t.leaves[i].code, t.leaves[i].length);
    printf("\n");
  }
}

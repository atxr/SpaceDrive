#include "libmineziper_huffman_tree.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tree build_tree(char* bit_lenghts, int size)
{
  assert(size < 0x10000);

  unsigned short* order_codes = sort_char(bit_lenghts, size);

  tree out;
  out.size = size;
  out.min = bit_lenghts[order_codes[0]];
  out.max = bit_lenghts[order_codes[size - 1]];
  out.leaves = malloc(sizeof(leaf) * size);

  int code = 0;
  int code_incr = 0;
  int last_bit_length = 0;
  short i = 0;
  unsigned short len_bit, j;

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

unsigned short* sort_char(const unsigned char* arr, int size)
{
  unsigned char MAX = 0xff;

  unsigned short* out = malloc(sizeof(int) * size);
  unsigned char* buf = malloc(size);
  memcpy(buf, arr, sizeof(char) * size);

  for (short k = 0; k < size; k++)
  {
    int min = 0;
    for (short i = 0; i < size; i++)
    {
      if (buf[min] == MAX || buf[i] < arr[min])
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

unsigned int* sort_int(const unsigned int* arr, int size)
{
  unsigned int MAX = 0xffffffff;

  unsigned int* out = malloc(sizeof(int) * size);
  unsigned int* buf = malloc(sizeof(int) * size);
  memcpy(buf, arr, sizeof(int) * size);

  for (int k = 0; k < size; k++)
  {
    int min = 0;
    for (int i = 0; i < size; i++)
    {
      if (buf[min] == MAX || buf[i] < arr[min])
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

int next_token(bitstream* bs, tree t)
{
  // TODO handle if tmin is null elsewhere
  int init = 0;
  if (t.min > 0)
  {
    init = t.min - 1;
  }

  unsigned int base = get_bits(bs, init);

  for (int k = t.min; k <= t.max; k++)
  {
    base += get_bits(bs, 1) << (k - 1);

    for (int i = 0; i < t.size; i++)
    {
      if (t.leaves[i].length == k && t.leaves[i].code == base)
        return t.leaves[i].litteral;
    }
  }

  return -1;
}

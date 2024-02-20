#include "libmineziper_bitstream.h"

#include <assert.h>

unsigned int get_bits(bitstream* bs, unsigned int bit_num)
{
  if (bit_num > 32)
    return -1;

  if (bit_num == 0)
    return 0;

  int out;
  if (bs->current_bit_offset + bit_num <= 8)
  {
    int mask = (1 << bit_num) - 1;

    out = (bs->data[bs->current_data_offset] >> bs->current_bit_offset) & mask;

    bs->current_bit_offset += bit_num;
    if (bs->current_bit_offset == 8)
    {
      bs->current_bit_offset = 0;
      bs->current_data_offset++;
    }
  }
  else
  {
    int diff = 8 - bs->current_bit_offset;
    int next = (bs->current_bit_offset + bit_num) % 8;
    out = get_bits(bs, diff);
    out += get_bits(bs, next) << diff;
  }

  return out;
}

unsigned int reverse(unsigned int x, unsigned int numBits)
{

  assert(x < (unsigned int) (1 << numBits));

  // fast lookup if it fits in a byte
  if (numBits == 8)
    return reverse8[(unsigned char) x];

  if (numBits < 8)
    return reverse8[(unsigned char) x] >> (8 - numBits);

  // more than a byte, slower
  unsigned int result = 0;
  for (unsigned int i = 0; i < numBits; i++)
  {
    result <<= 1;
    result |= x & 1;
    x >>= 1;
  }
  return result;
}

int get_bit_length(int x)
{
  int n = 0;
  while (x != 0)
  {
    x >> 1;
    n++;
  }
  return n;
}

void print_bits(int x, int bit_num)
{
  for (int i = bit_num - 1; i >= 0; i--)
  {
    printf("%c", 0b1 & (x >> i) ? '1' : '0');
  }
  printf("(%d,%d)", x, bit_num);
}

bitstream init_bitstream(char* data, int size, int last_bit_offset)
{
  bitstream bs = {
      .current_bit_offset = 0,
      .current_data_offset = 0,
      .data = data,
      .data_size = size,
      .last_bit_offset = last_bit_offset};

  return bs;
}
#ifndef LIBMINEZIPER_BITSTREAM_H
#define LIBMINEZIPER_BITSTREAM_H

typedef struct bitstream
{
  unsigned char *data;
  int data_size;        // size of 'data' array
  int last_bit_offset;  // last bit in the stream

  int current_data_offset;  // position in 'data', i.e.
                            // data[current_data_offset] is current
                            // reading/writing byte
  int current_bit_offset;   // which bit we are currently reading/writing
} bitstream;

unsigned int get_bits(bitstream *bs, unsigned int size);
void print_bits(int x, int size);
unsigned int reverse(unsigned int x, unsigned int numBits);
bitstream init_bitstream(char *data, int size, int last_bit_offset);

#endif
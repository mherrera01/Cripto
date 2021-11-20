#ifndef BITS_H
#define BITS_H

#define BYTE_SIZE 8 // Número de bits en cada byte
#define ASCII_0 48 // Caracter '0' en código ASCII
#define ASCII_1 49 // Caracter '1' en código ASCII

typedef struct _Bits Bits;

Bits* init_bits(int size);

void destroy_bits(Bits* bits);

Bits* copy_bits(Bits* bits);

int check_bits(Bits* bits);

int set_byte_to_value(Bits* bits, int byte, unsigned char value);

unsigned char get_byte_value(Bits* bits, int byte);

int split_bits(Bits* original, int split, Bits** part1, Bits** part2);

void print_bits(Bits* bits);

#endif
#ifndef BITS_H
#define BITS_H

#define BYTE_SIZE 8 // Número de bits en cada byte
#define ASCII_0 48 // Caracter '0' en código ASCII
#define ASCII_1 49 // Caracter '1' en código ASCII

typedef struct _Bits Bits;

// Inicializa una variable de tipo bits con tamaño size. En caso de error, devuelve NULL.
Bits* init_bits(int size);

// Libera la memoria de un objeto bits.
void destroy_bits(Bits* bits);

// Copia el contenido de una variable bits a otra y devuelve un puntero a ella. En caso de error, devuelve NULL.
Bits* copy_bits(Bits* bits);

// Revisa que el objeto bits tenga valores correctos. Devuelve 0 si es correcto y -1 si hay algún error.
int check_bits(Bits* bits);

// Setters -----------
int set_bit(Bits* bits, int index, unsigned char value);
int set_bits_to_zero(Bits* bits);
int set_bits_decimal_value(Bits* bits, unsigned long long value);
int set_hex_to_value(Bits* bits, int hex, unsigned char value);
int set_byte_to_value(Bits* bits, int byte, unsigned char value);
int set_bits_desKey_value(Bits *bits);

// Getters -----------
unsigned char get_bit(Bits* bits, int index);
int get_bits_size(Bits* bits);
unsigned long long get_bits_decimal_value(Bits *bits);
unsigned char get_byte_value(Bits* bits, int byte);

// Divide los N bits en dos grupos de tamaño split y N-split. En caso de error devuelve -1 y en caso de éxito devuelve 0.
int split_bits(Bits* original, int split, Bits** part1, Bits** part2);

// Une el contenido de dos variables de tipo Bits en una sola que devuelve. En caso de error, devuelve NULL.
Bits* merge_bits(Bits* part1, Bits* part2);

// Devuelve el resultado de la operación XOR entre dos operandos dados o NULL en caso de error.
Bits *xor_bits(Bits *operand1, Bits *operand2);

// Imprime por la terminal los bits en binario.
void print_bits(Bits* bits);

// Imprime por la terminal los bits en hexadecimal.
void print_bits_hex(Bits* bits);

#endif
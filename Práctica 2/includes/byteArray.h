#ifndef BYTEARRAY_H
#define BYTEARRAY_H

typedef struct _ByteArray ByteArray;

// Liberamos la memoria de la estructura
void destroy_byteArray(ByteArray *byteArray);

// Inicializamos memoria para una lista de size bytes
ByteArray *init_byteArray(int size);

/**
 * Asignamos un valor positivo aleatorio a cada byte con
 * su correspondiente bit de paridad impar.
 *
 * Devuelve -1 en caso de error y 0 en caso contrario.
 */
int set_byteArray_desKey_value(ByteArray *byteArray);

/**
 * Asignamos un valor decimal positivo a la lista de bytes.
 *
 * Devuelve -1 en caso de error y 0 en caso contrario.
 */
int set_byteArray_to_value(ByteArray *byteArray, unsigned long long value);

/**
 * Asignamos un valor al byte en la posición dada de la lista.
 *
 * Devuelve -1 en caso de error y 0 en caso contrario.
 */
int set_byteArray_byte(ByteArray* byteArray, char *byte, int index);

/**
 * Asignamos un valor decimal al byte en la posición dada de la lista.
 *
 * Devuelve -1 en caso de error y 0 en caso contrario.
 */
int set_byteArray_byte_value(ByteArray* byteArray, unsigned char byteValue, int index);

// Devuelve el valor decimal positivo de la lista de bytes o 0 en caso de error.
unsigned long long get_byteArray_value(ByteArray *byteArray);

// Devuelve el byte de una posición dada de la lista o NULL en caso de error.
char* get_byteArray_byte(ByteArray* byteArray, int index);

// Devuelve el valor decimal de una posición dada de la lista o 0 en caso de error.
unsigned char get_byteArray_byte_value(ByteArray* byteArray, int index);

// Devuelve el tamaño de la lista de bytes o -1 en caso de error.
int get_byteArray_size(ByteArray* byteArray);

/**
 * Divide el array de bytes de tamaño N en dos arrays de tamaño split y N-split.
 * Los arrays del resultado deben pasarse sin inicializar.
 *
 * Devuelve -1 en caso de error y 0 en caso contrario.
 */
int split_bytearray(ByteArray* original, int split, ByteArray* part1, ByteArray* part2);

/**
 * Imprimimos en pantalla la lista de bytes.
 * 
 * Devuelve -1 en caso de error y 0 en caso contrario.
 */
int print_byteArray(ByteArray *byteArray);

#endif
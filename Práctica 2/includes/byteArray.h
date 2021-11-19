#ifndef BYTEARRAY_H
#define BYTEARRAY_H

typedef struct _ByteArray ByteArray;

// Liberamos la memoria de la estructura
void destroy_byteArray(ByteArray *byteArray);

// Inicializamos memoria para una lista de size bytes
ByteArray *init_byteArray(int size);

/**
 * Asignamos un valor decimal positivo a la lista de bytes.
 *
 * Devuelve -1 en caso de error y 0 en caso contrario.
 */
int set_byteArray_to_value(ByteArray *byteArray, unsigned long long value);

// Devuelve el valor decimal positivo de la lista de bytes o 0 en caso contrario.
unsigned long long get_byteArray_value(ByteArray *byteArray);

/**
 * Imprimimos en pantalla la lista de bytes.
 * 
 * Devuelve -1 en caso de error y 0 en caso contrario.
 */
int print_byteArray(ByteArray *byteArray);

#endif
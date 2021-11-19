#ifndef BYTE_H
#define BYTE_H

#define BYTE_SIZE 8 // Número de bits en cada byte
#define ASCII_0 48 // Caracter '0' en código ASCII
#define ASCII_1 49 // Caracter '1' en código ASCII

// Inicializamos memoria para el byte
char* init_byte();

/**
 * Asignamos un valor decimal positivo al byte.
 * 
 * Devuelve -1 en caso de error y 0 en caso contrario.
 */
int set_byte_to_value(char* byte, unsigned char value);

// Devuelve el valor decimal positivo del byte o 0 en caso contrario.
unsigned char get_byte_value(char* byte);

#endif
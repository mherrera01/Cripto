#ifndef BYTE_H
#define BYTE_H

#define BYTE_SIZE 8 // Número de bits en cada byte
#define ASCII_0 48 // Caracter '0' en código ASCII
#define ASCII_1 49 // Caracter '1' en código ASCII

// Inicializamos memoria para el byte
char* init_byte();

/**
 * Asigna al byte el valor aleatorio de 7 bits dado y pone el bit paridad
 * en el menos significativo.
 * 
 * Para la paridad impar se suman los bits cuyo valor es uno, si da un
 * número impar de bits, entonces el bit de paridad (impar) es cero. Y si
 * la suma de los bits cuyo valor es uno es par, entonces el bit de paridad
 * (impar) se establece en uno, haciendo impar la cuenta total de bits uno.
 * 
 * Devuelve -1 en caso de error o 0 en caso contrario.
 */
int set_byte_desKey_value(char *byte, unsigned char value);

/**
 * Asignamos un valor decimal positivo al byte.
 * 
 * Devuelve -1 en caso de error y 0 en caso contrario.
 */
int set_byte_to_value(char* byte, unsigned char value);

// Devuelve el valor decimal positivo del byte o 0 en caso contrario.
unsigned char get_byte_value(char* byte);

#endif
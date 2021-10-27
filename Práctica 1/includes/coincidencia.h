#ifndef COINCIDENCIA_H
#define COINCIDENCIA_H

#define COINCIDENCE_ALPHABET_SIZE 26 // Tamaño del alfabeto con la tabla de frecuencias (A-Z)

// Libera la memoria de las subcadenas de un texto
void free_substrings(char **substrings, int keyLength);

/**
 * Divide una cadena de texto dada por el tamaño de la clave.
 * 
 * Devuelve la cadena dividida o NULL en caso de error.
 */
char **divide_strings(char* string, int keyLength);

// Devuelve la letra de la clave con la que se ha cifrado la cadena texto
char get_caesar_key(char* string);

/**
 * Calcula el índice de coincidencia en la cadena de texto con el tamaño de la
 * clave. 0 para hacer el cálculo con la tabla de frecuencias en inglés y 1
 * en español.
 * 
 * Devuelve el índice de coincidencia entre 0.0 y 1.0 o -1 en caso de error
 */
double check_key_length_IC(char* string, int keyLength, int language);

#endif
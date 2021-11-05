#ifndef ALPHABET_H
#define ALPHABET_H

#define DEFAULT_ALPHABET_SIZE 79

/* Devuelve una letra dado el código de esa letra en nuestro alfabeto.*/
char get_letter(int l);

/* Devuelve el código de una letra en nuestro alfabeto dado el caracter.*/
int get_letter_code(char c);

/**
 * Carga n caracteres del alfabeto en el archivo con nombre fileName.
 * Si no hay suficientes caracteres se carga el alfabeto entero.
 * 
 * Devuelve -1 en caso de error y el número de caracteres cargados en caso contrario.
 */
int load_alphabet(char *fileName, int n);

/* Libera la memoria del alfabeto.*/
void destroy_alphabet();

#endif
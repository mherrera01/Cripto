#ifndef ALPHABET_H
#define ALPHABET_H

/* Devuelve una letra dado el código de esa letra en nuestro alfabeto.*/
char get_letter(int l);

/* Devuelve el código de una letra en nuestro alfabeto dado el caracter.*/
int get_letter_code(char c);

/**
 * Carga n caracteres del alfabeto en el archivo alphabet/alphabet.txt.
 * Si no hay suficientes caracteres se carga el alfabeto entero.
 * 
 * Devuelve -1 en caso de error y 0 en caso contrario.
 */
int load_alphabet(int n);

/* Libera la memoria del alfabeto.*/
void destroy_alphabet();

#endif
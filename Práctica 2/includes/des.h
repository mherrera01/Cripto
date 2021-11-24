#ifndef DES_H
#define DES_H

// Devuelve una clave aleatoria de 64 bits (56 bits + 8 bits paridad) o NULL en caso de error.
Bits *generate_random_desKey();

/**
 * Permutación inicial de los 64 bits de entrada.
 * 
 * Devuelve NULL en caso de error o la clave permutada en caso contrario.
 */
Bits *initial_permutation(Bits *message);

/**
 * Calcula el resultado de una ronda dada una clave y lo devuelve
 * en el mensaje 
 * 
 * Devuelve -1 en caso de error 0 en caso contrario.
 */
int compute_round(Bits *message, Bits *key);

// Libera la memoria de las 16 subclaves necesarias para el cifrado/descifrado
void free_des_subkeys(Bits **subkeys);

// Devuelve la lista de las 16 subclaves necesarias para el cifrado/descifrado o NULL en caso de error.
Bits **get_des_subkeys(Bits *key);

// Hace un swap y permuta el mensaje de salida de la última ronda
Bits *compute_last_round(Bits *message);

/**
 * Cifra con el algoritmo des un mensaje de 64 bits con una
 * clave de también 64 bits.
 * 
 * Devuelve el mensaje cifrado en 8 bytes o NULL en caso de error.
 */
Bits *des_encrypt(Bits *message, Bits *key);

/**
 * Descifra con el algoritmo des un mensaje de 64 bits aplicando
 * las subclaves en orden inverso.
 * 
 * Devuelve el mensaje descifrado en 8 bytes o NULL en caso de error.
 */
Bits *des_decrypt(Bits *message, Bits *key);

#endif
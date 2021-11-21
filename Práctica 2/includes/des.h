#ifndef DES_H
#define DES_H

// Devuelve una clave aleatoria de 64 bits (56 bits + 8 bits paridad) o NULL en caso de error.
Bits *generate_random_desKey();

/**
 * Cifra con el algoritmo des un mensaje de 64 bits con una
 * clave de también 64 bits.
 * 
 * Devuelve el mensaje cifrado en 8 bytes o NULL en caso de error.
 */
Bits *des_encrypt(Bits *message, Bits *key);

#endif
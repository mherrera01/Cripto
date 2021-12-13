#ifndef RANDOMPRIME_H
#define RANDOMPRIME_H

#include <gmp.h>

// Información de la ejecución del generador de primos a mostrar al usuario
typedef struct _RandomPrimeInfo RandomPrimeInfo;

int get_testResult(RandomPrimeInfo *info);
int get_testGMPResult(RandomPrimeInfo *info);
int get_nTimes(RandomPrimeInfo *info);
char *get_testResult_string(int testResult);

/**
 * Asigna en las variables m y n el resultado de la operación n-1 = 2^k * m.
 * Los valores en el intervalo [1, bits-1] serán probados en k hasta que se
 * encuentre la solución, siendo m impar.
 * 
 * Devuelve 0 si se ha encontrado la solución o -1 en caso de error.
 */
int calculate_mk(mpz_t *m, mpz_t *k, mpz_t *n, int bits);

/**
 * Asigna en prime un número aleatorio primo de n bits con una probabilidad
 * de equivocación de sec. Recibe un estado aleatorio para generar un número
 * candidato en cada ejecución.
 * 
 * Devuelve la información resultante de la generación del posible primo o
 * NULL en caso de error.
 */
RandomPrimeInfo *generate_random_prime(mpz_t *prime, int bits, double sec, gmp_randstate_t randState);

#endif

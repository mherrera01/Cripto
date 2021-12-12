#ifndef MILLERRABIN_H
#define MILLERRABIN_H

#include <gmp.h>

// Probabilidad de equivocación mínima en el algoritmo de miller-rabin
#define MIN_ERROR_ESTIMATE 0.000000000000001

/**
 * Devuelve el número de veces que son necesarias ejecutar miller-rabin para
 * que un número de N bits tenga una probabilidad de equivocación específica.
 * El resultado siempre es redondeado hacia arriba en busca de más seguridad.
 */
int get_ntimes_estimate_millerRabin(int bits, double error);

/**
 * Devuelve la probabilidad de que un número de N bits sea compuesto
 * después de superar n veces el test de miller-rabin.
 */
double get_error_estimate_millerRabin(int bits, int ntimes);

/**
 * Asigna en las variables m y n el resultado de la operación n-1 = 2^k * m.
 * Los valores en el intervalo [1, bits-1] serán probados en k hasta que se
 * encuentre la solución, siendo m impar.
 * 
 * Devuelve 0 si se ha encontrado la solución o -1 en caso de error.
 */
int calculate_mk(mpz_t *m, mpz_t *k, mpz_t *n, int bits);

/**
 * Comprueba si el número pasado como argumento es un posible primo mediante el
 * algoritmo de miller-rabin.
 * 
 * Devuelve 1 si el número es un posible primo, 0 en caso contrario y -1 en caso de error.
 */
int check_prime_millerRabin(mpz_t *m, mpz_t *k, mpz_t *n);

#endif

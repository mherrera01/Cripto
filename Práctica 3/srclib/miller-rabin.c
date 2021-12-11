#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../includes/miller-rabin.h"

double get_error_estimate_millerRabin(int bits, int ntimes) {
    double denominator = 0.0;

    // 1 + (4^ntimes / (bits * ln2))
    denominator = 1.0 + (pow(4.0, ntimes) / (bits * log(2.0)));
    
    // Estimación de la equivocación del algoritmo miller-rabin
    return 1.0 / denominator;
}

int calculate_mk(mpz_t *m, mpz_t *k, mpz_t *n, int bits) {
    // Control de errores
    if (m == NULL || k == NULL || n == NULL || bits <= 0) return -1;

    return 0;
}

int check_prime_millerRabin(mpz_t *m, mpz_t *k, mpz_t *n) {
    // Control de errores
    if (m == NULL || k == NULL || n == NULL) return -1;

    return 1;
}

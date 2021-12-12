#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../includes/miller-rabin.h"

int get_ntimes_estimate_millerRabin(int bits, double error) {
    double numerator = 0.0, denominator = 0.0;

    // Casos extremos
    if (error <= 0.0) error = MIN_ERROR_ESTIMATE;
    if (error >= 1.0) return 0;

    // ln ((bits * ln2 * (1-error)) / error)
    numerator = log((bits * log(2.0) * (1.0-error)) / error);

    // ln4
    denominator = log(4.0);

    // Número de veces que son necesarias ejecutar miller-rabin
    return (int) ceil(numerator/denominator);
}

double get_error_estimate_millerRabin(int bits, int ntimes) {
    double denominator = 0.0;

    // 1 + (4^ntimes / (bits * ln2))
    denominator = 1.0 + (pow(4.0, ntimes) / (bits * log(2.0)));
    
    // Estimación de la equivocación del algoritmo miller-rabin
    return 1.0 / denominator;
}

int calculate_mk(mpz_t *m, mpz_t *k, mpz_t *n, int bits) {
    int i;
    mpz_t result, base, power, checkm;

    // Control de errores
    if (m == NULL || k == NULL || n == NULL || bits <= 0) return -1;

    // Inicializamos las variables
    mpz_init(result);
    mpz_init(base);
    mpz_init(power);
    mpz_init(checkm);

    // Asignamos los valores iniciales
    mpz_set(result, *n);
    mpz_sub_ui(result, result, 1); // n-1
    mpz_set_ui(base, 2);

    // Probamos diferentes valores hasta resolver la operación
    for (i = 1; i < bits-1; i++) {
        // Calculamos m a partir del k asignado
        mpz_pow_ui(power, base, i);
        mpz_fdiv_q(*m, result, power); // m = (n-1) / 2^k

        // Comprobamos si m es impar
        mpz_fdiv_r_ui(checkm, *m, 2);
        if (mpz_cmp_si(checkm, 0) != 0) {
            // k y m calculados
            mpz_set_ui(*k, i);
            break;
        }
    }

    // Liberamos la memoria
    mpz_clear(result);
    mpz_clear(base);
    mpz_clear(power);
    mpz_clear(checkm);

    return 0;
}

int check_prime_millerRabin(mpz_t *m, mpz_t *k, mpz_t *n) {
    // Control de errores
    if (m == NULL || k == NULL || n == NULL) return -1;

    return 1;
}

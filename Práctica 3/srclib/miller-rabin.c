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

int check_prime_millerRabin(mpz_t *m, mpz_t *k, mpz_t *n, gmp_randstate_t randState) {
    int i;
    mpz_t a, top, x;

    // Control de errores
    if (m == NULL || k == NULL || n == NULL) return -1;

    // Miller-rabin se ejecuta en números mayores que 3
    if (mpz_cmp_si(*n, 3) <= 0) return 1;

    // Inicializamos las variables
    mpz_init(a);
    mpz_init(top);
    mpz_init(x);

    // Asignamos los valores inciales
    mpz_set(top, *n);
    mpz_sub_ui(top, top, 1);

    // Generamos un número entero aleatorio entre 1 y n-1
    mpz_urandomm(a, randState, top);
    mpz_add_ui(a, a, 1);

    // Hacemos la operación x = a^m mod n
    mpz_powm(x, a, *m, *n);

    // Comprobamos si es un posible primo (x == 1 | x == n-1)
    if (mpz_cmp_si(x, 1) == 0 || mpz_cmp(x, top) == 0) {
        mpz_clear(a);
        mpz_clear(top);
        mpz_clear(x);
        return 1; // Posible primo
    }

    // De 1 a k-1
    for (i = 1; i < mpz_get_ui(*k); i++) {
        // x = x^2 mod n
        mpz_mul(x, x, x);
        mpz_mod(x, x, *n);

        // x == 1
        if (mpz_cmp_si(x, 1) == 0) {
            mpz_clear(a);
            mpz_clear(top);
            mpz_clear(x);
            return 0; // No se cumple la propiedad; número compuesto

        // x == n-1
        } else if (mpz_cmp(x, top) == 0) {
            mpz_clear(a);
            mpz_clear(top);
            mpz_clear(x);
            return 1; // Posible primo
        }
    }

    // Liberamos memoria
    mpz_clear(a);
    mpz_clear(top);
    mpz_clear(x);

    // Número compuesto
    return 0;
}

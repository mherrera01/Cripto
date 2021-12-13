#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/randomPrime.h"
#include "../includes/miller-rabin.h"
#include "../includes/firstPrimes.h"

struct _RandomPrimeInfo {
    int testResult; // Resultado de nuestro test. 1 si es posible primo ó 0 si es compuesto
    int testGMPResult; // Resultado de GMP. 2 si es primo, 1 si es posible primo ó 0 si es compuesto
    int nTimes; // Número de veces que pasa el test para generar el número de n bits
};

RandomPrimeInfo *initialize_RandomPrimeInfo() {
    RandomPrimeInfo *info = NULL;

    // Reservamos memoria para la estructura
    info = (RandomPrimeInfo *) malloc (sizeof(RandomPrimeInfo));
    if (info == NULL) return NULL;

    // Asignamos los valores iniciales
    info->testResult = -1;
    info->testGMPResult = -1;
    info->nTimes = -1;

    return info;
}

// --------------- RandomPrimeInfo structure getters

int get_testResult(RandomPrimeInfo *info) {
    if (info == NULL) return -1;
    return info->testResult;
}

int get_testGMPResult(RandomPrimeInfo *info) {
    if (info == NULL) return -1;
    return info->testGMPResult;
}

int get_nTimes(RandomPrimeInfo *info) {
    if (info == NULL) return -1;
    return info->nTimes;
}

char *get_testResult_string(int testResult) {
    if (testResult == -1) {
        return "None";
    } else if (testResult == 0) {
        return "No primo";
    } else if (testResult == 1) {
        return "Posible primo";
    } else if (testResult == 2) {
        return "Primo";
    }

    return "Error";
}

// --------------- Funciones para generar primos aleatorios

int calculate_mk(mpz_t *m, mpz_t *k, mpz_t *n, int bits) {
    int i, solutionFound = 0;
    mpz_t result, base, power, checkm;

    // Control de errores
    if (m == NULL || k == NULL || n == NULL || bits <= 1) return -1;

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
    for (i = 1; i < bits; i++) {
        // Calculamos m a partir del k asignado
        mpz_pow_ui(power, base, i);
        mpz_fdiv_q(*m, result, power); // m = (n-1) / 2^k

        // Comprobamos si m es impar
        mpz_fdiv_r_ui(checkm, *m, 2);
        if (mpz_cmp_si(checkm, 0) != 0) {
            // k y m calculados
            mpz_set_ui(*k, i);
            solutionFound = 1;
            break;
        }
    }

    // Liberamos la memoria
    mpz_clear(result);
    mpz_clear(base);
    mpz_clear(power);
    mpz_clear(checkm);

    if (!solutionFound) return -1;
    return 0;
}

/**
 * Devuelve 1 si el número pasado como argumento es divisible entre alguno
 * de los primos del 1 al 2000 y devuelve 0 en caso contrario.
 * 
 * En caso de error, devuelve -1.
 */
int check_first_primes_division(mpz_t *randN) {
    int i;
    mpz_t prime, remainder;

    // Control de errores
    if (randN == NULL) return -1;

    // Inicializamos las variables
    mpz_init(prime);
    mpz_init(remainder);

    // Comprobamos con cada primo hasta el 2000 si el número aleatorio es divisible
    for (i = 0; i < N_PRIMES; i++) {
        // Asignamos el número primo
        mpz_set_ui(prime, FIRST_PRIMES[i]);

        // Devuelve un valor positivo si randN > prime
        if (mpz_cmp(*randN, prime) > 0) {
            // Calculamos el resto de la división
            mpz_fdiv_r(remainder, *randN, prime);

            // Si el resto es 0, el número aleatorio es compuesto
            if (mpz_cmp_si(remainder, 0) == 0) {
                mpz_clear(prime);
                mpz_clear(remainder);
                return 1;
            }
        } else break;
    }

    // Liberamos memoria
    mpz_clear(prime);
    mpz_clear(remainder);

    return 0;
}

RandomPrimeInfo *generate_random_prime(mpz_t *prime, int bits, double sec, gmp_randstate_t randState) {
    int i, primesDiv, isCompound, testResult, testNtimes, generateRand = 1;
    mpz_t m, k;
    RandomPrimeInfo *resultInfo = NULL;

    // Control de errores
    if (prime == NULL || bits <= 1 || sec < 0 || sec > 1) return NULL;

    // Reservamos memoria para la información de la generación de primos
    resultInfo = initialize_RandomPrimeInfo();
    if (resultInfo == NULL) return NULL;

    // Inicializamos memoria para las variables necesarias del algoritmo
    mpz_init(m);
    mpz_init(k);

    // Calculamos cuántas veces es necesario ejecutar miller-rabin
    testNtimes = get_ntimes_estimate_millerRabin(bits, sec);
    if (testNtimes < 0) testNtimes = 0;
    resultInfo->nTimes = testNtimes;

    while(generateRand) {
        // Generamos un número positivo aleatorio entre 0 y 2^(n-1)
        mpz_urandomb(*prime, randState, bits);

        // Ponemos a 1 el bit más significativo para asegurar el correcto tamaño de bits
        mpz_setbit(*prime, bits-1);

        // Ponemos a 1 el bit menos significativo para asegurar que es impar
        mpz_setbit(*prime, 0);
        generateRand = 0;

        while(1) {
            // Dividimos el número aleatorio generado por los primos menores que 2000 para descartar compuestos
            primesDiv = check_first_primes_division(prime);
            if (primesDiv == -1) {
                printf("Error: No se ha podido comprobar si el número aleatorio generado es divisible entre los primos del 1 al 2000.\n");

                mpz_clear(m);
                mpz_clear(k);
                free(resultInfo);
                return NULL;
            } else if (primesDiv) {
                // Número compuesto; generamos otro aleatorio
                generateRand = 1;
                break;
            }

            // Comprobamos si no hace falta hacer miller-rabin
            if (testNtimes == 0) break;

            // Calculamos las variables m y k necesarias para el algoritmo de miller-rabin
            if (calculate_mk(&m, &k, prime, bits) == -1) {
                printf("Error: No se han podido calcular las variables m y k necesarias para el algoritmo de miller-rabin.\n");

                mpz_clear(m);
                mpz_clear(k);
                free(resultInfo);
                return NULL;
            }

            // Aplicamos el test de primalidad con el algoritmo de miller-rabin las veces necesarias
            isCompound = 0;
            for (i = 0; i < testNtimes; i++) {
                testResult = check_prime_millerRabin(&m, &k, prime, randState);
                if (testResult == -1) {
                    printf("Error: No se ha podido aplicar el test de primalidad al número aleatorio generado con el algoritmo de miller-rabin.\n");

                    mpz_clear(m);
                    mpz_clear(k);
                    free(resultInfo);
                    return NULL;
                } else if (!testResult) {
                    // Número compuesto
                    isCompound = 1;
                    break;
                }
            }

            // Guardamos el resultado de nuestro test con el algoritmo de miller-rabin
            resultInfo->testResult = (isCompound) ? 0 : 1;

            // Comprobamos con gmp si el número generado aleatoriamente es un posible primo
            resultInfo->testGMPResult = mpz_probab_prime_p(*prime, testNtimes);

            if (isCompound) {
                // El número aleatorio generado no pasa los tests
                mpz_add_ui(*prime, *prime, 2);

                // Con base 2 devuelve la localización del bit 1 más significativo del número
                if (mpz_sizeinbase(*prime, 2) > bits) {
                    // Generamos otro número aleatorio
                    generateRand = 1;
                    break;
                } else {
                    // Si n+2 no ocupa todos los bits seguimos probando
                    continue;
                }
            } else {
                // Posible primo
                break;
            }
        }
    }

    // Liberamos memoria
    mpz_clear(m);
    mpz_clear(k);

    return resultInfo;
}

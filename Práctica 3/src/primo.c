#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gmp.h>
#include <sys/random.h>

#include "../includes/miller-rabin.h"
#include "../includes/firstPrimes.h"

/*
#define _POSIX_C_SOURCE 200809L // Necesario para usar la función clock_gettime si se compila con ANSI
*/

#define NS_PER_SEC 1000000000 // Nanosegundos en 1 segundo

// Información de la ejecución del programa a mostrar al usuario
typedef struct ProgramInfo {
    int testResult; // Resultado de nuestro test. 1 si es posible primo ó 0 si es compuesto
    int testGMPResult; // Resultado de GMP. 2 si es primo, 1 si es posible primo ó 0 si es compuesto
    int nTimes; // Número de veces que pasa el test para generar el número de n bits
} ProgramInfo;

ProgramInfo *initialize_ProgramInfo() {
    ProgramInfo *info = NULL;

    // Reservamos memoria para la estructura
    info = (ProgramInfo *) malloc (sizeof(ProgramInfo));
    if (info == NULL) return NULL;

    // Asignamos los valores iniciales
    info->testResult = -1;
    info->testGMPResult = -1;
    info->nTimes = -1;

    return info;
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

double get_execution_time(struct timespec start, struct timespec end) {
    double seconds, nanoseconds;

    // Calculamos la diferencia de los tiempos en segundos y nanosegundos
    seconds = end.tv_sec - start.tv_sec;
    nanoseconds = end.tv_nsec - start.tv_nsec;

    // Ajustamos los nanosegundos para que sea positivo
    if (start.tv_nsec > end.tv_nsec) {
	    --seconds;
	    nanoseconds += NS_PER_SEC;
    }

    return seconds + (nanoseconds / NS_PER_SEC);
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

ProgramInfo *generate_random_prime(mpz_t *prime, int bits, double sec) {
    int i, primesDiv, isCompound, testResult, testNtimes, generateRand = 1;
    gmp_randstate_t randState;
    mpz_t m, k;
    unsigned long seed;
    ProgramInfo *resultInfo = NULL;

    // Control de errores
    if (prime == NULL || bits <= 1 || sec < 0 || sec > 1) return NULL;

    // Reservamos memoria para la información de la generación de primos
    resultInfo = initialize_ProgramInfo();
    if (resultInfo == NULL) return NULL;

    // Generamos una semilla aleatoria en cada ejecución del programa
    if (getrandom(&seed, sizeof(unsigned long), GRND_NONBLOCK) == -1) {
        free(resultInfo);
        return NULL;
    }

    // Inicializamos memoria para la semilla generada
    gmp_randinit_mt(randState);
    gmp_randseed_ui(randState, seed);

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

                gmp_randclear(randState);
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

                gmp_randclear(randState);
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

                    gmp_randclear(randState);
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
    gmp_randclear(randState);
    mpz_clear(m);
    mpz_clear(k);

    return resultInfo;
}

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./primo {-b bits} {-p sec} [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-b número máximo de bits significativos que tendrá el número generado\n");
    printf("-p probabilidad de equivocación del algoritmo que especifica la seguridad que debería tener el primo generado en el test. Ej: 0.05\n");
    printf("-o fichero de salida\n");
    printf("---------------------------\n");
}

// Cerramos el archivo de salida abierto previamente
void close_file(FILE *output) {
    if (output == NULL) return;

    // Cerramos el archivo si no es stdout
    if (output != stdout) fclose(output);
}

int main(int argc, char *argv[]) {
    mpz_t prime;
    int i, bits = 0;
    double sec = 0.0, executionTime = 0.0;
    char *convertToLong;
    struct timespec start, end;
    ProgramInfo *generatePrimeResult = NULL;
    FILE *output = stdout;

    // Inicializamos la variable con el número primo resultante
    mpz_init(prime);

    // Comprobar número de argumentos del usuario
    if (argc < 5 || argc > 7) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();

        mpz_clear(prime);
        return -1;
    }

    // Obtener parámetros del usuario
    // primo {-b bits} {-p sec} [-o fileout]
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-b") == 0) {
            // Número máximo de bits significativos que tendrá el número generado
            bits = (int) strtol(argv[++i], &convertToLong, 10);

            // Comprobamos si no se ha convertido ningún caracter o el valor no es válido
            if (argv[i] == convertToLong || bits <= 1) {
                printf("Error: El parámetro -b %s no es válido.\n", argv[i]);

                close_file(output);
                mpz_clear(prime);
                return -1;
            }

        } else if (strcmp(argv[i], "-p") == 0) {
            // Probabilidad de equivocación del algoritmo que especifica la seguridad que debería tener el primo generado en el test
            sec = strtod(argv[++i], &convertToLong);

            // Comprobamos si no se ha convertido ningún caracter o el valor no es válido
            if (argv[i] == convertToLong || strchr(argv[i], ',') != NULL || sec < 0 || sec > 1) {
                printf("Error: El parámetro -p %s no es válido.\n", argv[i]);
                printf("La probabilidad de equivocación del primo generado debe estar en el intervalo [0,1] ");
                printf("y tener el punto como separador decimal.\n");

                close_file(output);
                mpz_clear(prime);
                return -1;
            }

        // Parámetro de fichero de salida. Por defecto stdout
        } else if (strcmp(argv[i], "-o") == 0) {
            // Abrimos fichero de salida
            output = fopen(argv[++i], "w");
            if (output == NULL) {
                printf("Error: El archivo de salida %s no se ha podido encontrar.\n", argv[i]);

                close_file(output);
                mpz_clear(prime);
                return -1;
            }

        } else {
            printf("Error: Parámetro incorrecto %s.\n", argv[i]);
            print_help();

            close_file(output);
            mpz_clear(prime);
            return -1;
        }
    }

    printf("Generando el número primo...\n");

    // Empezamos a medir la velocidad de la generación de números primos
    if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
        printf("Error: No se ha podido empezar a medir la velocidad de la generación de números primos.\n");

        close_file(output);
        mpz_clear(prime);
        return -1;
    }

    // Generamos el número primo aleatorio de n bits con una cierta probabilidad de equivocación
    generatePrimeResult = generate_random_prime(&prime, bits, sec);
    if (generatePrimeResult == NULL) {
        printf("Error: No se ha podido generar el número primo aleatorio de %d bits con una equivocación de %lf.\n", bits, sec);

        close_file(output);
        mpz_clear(prime);
        return -1;
    }

    // Terminamos de medir la velocidad de la generación de números primos
    if (clock_gettime(CLOCK_REALTIME, &end) == -1) {
        printf("Error: No se ha podido terminar de medir la velocidad de la generación de números primos.\n");

        close_file(output);
        mpz_clear(prime);
        free(generatePrimeResult);
        return -1;
    }

    // Tiempo que tarda el programa en generar el número
    executionTime = get_execution_time(start, end);

    printf("OK: Número primo generado.\n");
    if (output == stdout) printf("\n");

    // Mostramos en el archivo de salida correspondiente los resultados de la ejecución del programa
    gmp_fprintf(output, "%Zd\n", prime);
    fprintf(output, "--------------------------\n");
    fprintf(output, "Resultado de nuestro test: %s\n", get_testResult_string(generatePrimeResult->testResult));
    fprintf(output, "Resultado de GMP: %s\n", get_testResult_string(generatePrimeResult->testGMPResult));
    fprintf(output, "Probabilidad de equivocación: %.15lf\n", sec);
    fprintf(output, "Número de veces que el número candidato ha pasado el test: %d\n", generatePrimeResult->nTimes);
    fprintf(output, "Tiempo de ejecución: %lfs\n", executionTime);

    // Cerramos el archivo si no es stdout
    close_file(output);

    // Liberamos la variable con el número primo resultante
    mpz_clear(prime);

    // Liberamos memoria
    free(generatePrimeResult);

    return 0;
}

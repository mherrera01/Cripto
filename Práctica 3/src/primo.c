#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gmp.h>
#include <sys/random.h>

#include "../includes/miller-rabin.h"
#include "../includes/firstPrimes.h"

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
            if (mpz_cmp_si(remainder, 0) == 0) return 1;
        } else break;
    }

    // Liberamos memoria
    mpz_clear(prime);
    mpz_clear(remainder);

    return 0;
}

double generate_random_prime(mpz_t *prime, int bits, double sec) {
    int primesDiv = 0, testResult = 0, generateRand = 1;
    gmp_randstate_t randState;
    mpz_t m, k;
    unsigned long seed;

    // Control de errores
    if (prime == NULL || bits <= 0 || sec < 0 || sec > 1) return -1.0;

    // Generamos una semilla aleatoria en cada ejecución del programa
    if (getrandom(&seed, sizeof(unsigned long), GRND_NONBLOCK) == -1) return -1.0;

    // Inicializamos memoria para la semilla generada
    gmp_randinit_mt(randState);
    gmp_randseed_ui(randState, seed);

    // Inicializamos memoria para las variables necesarias del algoritmo
    mpz_init(m);
    mpz_init(k);

    while(generateRand) {
        // Generamos un número positivo aleatorio entre 0 y 2^(n-1)
        // TODO: mpz_rrandomb. Entre 2^(n-1) y 2^n??
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
                return -1.0;
            } else if (primesDiv) {
                // Número compuesto; generamos otro aleatorio
                generateRand = 1;
                break;
            }

            // Calculamos las variables m y k necesarias para el algoritmo de miller-rabin
            if (calculate_mk(&m, &k, prime, bits) == -1) {
                printf("Error: No se han podido calcular las variables m y k necesarias para el algoritmo de miller-rabin.\n");

                gmp_randclear(randState);
                mpz_clear(m);
                mpz_clear(k);
                return -1.0;
            }

            // Aplicamos el test de primalidad con el algoritmo de miller-rabin
            testResult = check_prime_millerRabin(&m, &k, prime);
            if (testResult == -1) {
                printf("Error: No se ha podido aplicar el test de primalidad al número aleatorio generado con el algoritmo de miller-rabin.\n");

                gmp_randclear(randState);
                mpz_clear(m);
                mpz_clear(k);
                return -1.0;
            } else if (testResult) {
                // Posible primo
                break;
            } else {
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
            }
        }
    }

    // https://stackoverflow.com/questions/30942413/c-gmp-generating-random-number

    // Liberamos memoria
    gmp_randclear(randState);
    mpz_clear(m);
    mpz_clear(k);

    return 0.0;
}

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./primo {-b bits} {-p sec} [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-b número máximo de bits significativos que tendrá el número generado\n");
    printf("-p probabilidad de equivocación del algoritmo que especifica la seguridad que debería tener el primo generado en el test. Ej: 0,05\n");
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
    double sec = 0.0;
    char *convertToLong;
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
            if (argv[i] == convertToLong || bits <= 0) {
                printf("Error: El parámetro -b %s no es válido.\n", argv[i]);

                close_file(output);
                mpz_clear(prime);
                return -1;
            }

        } else if (strcmp(argv[i], "-p") == 0) {
            // Probabilidad de equivocación del algoritmo que especifica la seguridad que debería tener el primo generado en el test
            sec = strtod(argv[++i], &convertToLong);

            // Comprobamos si no se ha convertido ningún caracter o el valor no es válido
            if (argv[i] == convertToLong || sec < 0 || sec > 1) {
                printf("Error: El parámetro -p %s no es válido.\n", argv[i]);
                printf("La probabilidad de equivocación del primo generado debe estar en el intervalo [0,1].\n");

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

    generate_random_prime(&prime, bits, sec);
    gmp_printf("%Zd\n", prime);

    // Cerramos el archivo si no es stdout
    close_file(output);

    // Liberamos la variable con el número primo resultante
    mpz_clear(prime);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/random.h>
#include <gmp.h>
#include "../includes/randomPrime.h"

/*
#define _POSIX_C_SOURCE 200809L // Necesario para usar la función clock_gettime si se compila con ANSI
*/

#define NS_PER_SEC 1000000000 // Nanosegundos en 1 segundo

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
    RandomPrimeInfo *generatePrimeResult = NULL;
    gmp_randstate_t randState;
    unsigned long seed;
    FILE *output = stdout;

    // Generamos una semilla aleatoria en cada ejecución del programa
    if (getrandom(&seed, sizeof(unsigned long), GRND_NONBLOCK) == -1) {
        printf("Error: No se ha podido generar una semilla aleatoria para esta ejecución del programa.\n");
        return -1;
    }

    // Inicializamos memoria para la semilla generada
    gmp_randinit_mt(randState);
    gmp_randseed_ui(randState, seed);

    // Inicializamos la variable con el número primo resultante
    mpz_init(prime);

    // Comprobar número de argumentos del usuario
    if (argc < 5 || argc > 7) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();

        gmp_randclear(randState);
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
                gmp_randclear(randState);
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
                gmp_randclear(randState);
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
                gmp_randclear(randState);
                mpz_clear(prime);
                return -1;
            }

        } else {
            printf("Error: Parámetro incorrecto %s.\n", argv[i]);
            print_help();

            close_file(output);
            gmp_randclear(randState);
            mpz_clear(prime);
            return -1;
        }
    }

    printf("Generando el número primo...\n");

    // Empezamos a medir la velocidad de la generación de números primos
    if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
        printf("Error: No se ha podido empezar a medir la velocidad de la generación de números primos.\n");

        close_file(output);
        gmp_randclear(randState);
        mpz_clear(prime);
        return -1;
    }

    // Generamos el número primo aleatorio de n bits con una cierta probabilidad de equivocación
    generatePrimeResult = generate_random_prime(&prime, bits, sec, randState);
    if (generatePrimeResult == NULL) {
        printf("Error: No se ha podido generar el número primo aleatorio de %d bits con una equivocación de %lf.\n", bits, sec);

        close_file(output);
        gmp_randclear(randState);
        mpz_clear(prime);
        return -1;
    }

    // Terminamos de medir la velocidad de la generación de números primos
    if (clock_gettime(CLOCK_REALTIME, &end) == -1) {
        printf("Error: No se ha podido terminar de medir la velocidad de la generación de números primos.\n");

        close_file(output);
        gmp_randclear(randState);
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
    fprintf(output, "Resultado de nuestro test: %s\n", get_testResult_string(get_testResult(generatePrimeResult)));
    fprintf(output, "Resultado de GMP: %s\n", get_testResult_string(get_testGMPResult(generatePrimeResult)));
    fprintf(output, "Probabilidad de equivocación: %.15lf\n", sec);
    fprintf(output, "Número de veces que el número candidato ha pasado el test: %d\n", get_nTimes(generatePrimeResult));
    fprintf(output, "Tiempo de ejecución: %lfs\n", executionTime);

    // Cerramos el archivo si no es stdout
    close_file(output);

    // Liberamos la variable con el número primo resultante
    mpz_clear(prime);

    // Liberamos memoria
    gmp_randclear(randState);
    free(generatePrimeResult);

    return 0;
}

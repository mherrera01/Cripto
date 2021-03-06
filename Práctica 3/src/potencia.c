#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gmp.h>

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

void get_modular_power(mpz_t *result, mpz_t *base, mpz_t *exponent, mpz_t *module) {
    size_t l;
    long i;

    // Control de errores
    if (result == NULL || base == NULL || exponent == NULL || module == NULL) return;

    // Con base 2 devuelve la localización del bit 1 más significativo del exponente
    l = mpz_sizeinbase(*exponent, 2);

    // Asignamos el valor inicial de x
    mpz_set_str(*result, "1", 10);

    for (i = l-1; i>=0; i--) {
        // x = x^2 mod n
        mpz_mul(*result, *result, *result);
        mpz_mod(*result, *result, *module);

        // Comprobamos si el bit en la posición i del exponente es 1
        if (mpz_tstbit(*exponent, i)) {
            // x = x*b mod n
            mpz_mul(*result, *result, *base);
            mpz_mod(*result, *result, *module);
        }
    }
}

/* Devuelve el tiempo de ejecución de nuestra función de potenciación y asigna el resultado en
el mpz_t result pasado como argumento. */
double get_time_modular_power(mpz_t *result, mpz_t *base, mpz_t *exponent, mpz_t *module) {
    struct timespec start, end;
    double executionTime = 0.0;

    // Control de errores
    if (result == NULL || base == NULL || exponent == NULL || module == NULL) return -1.0;

    // Empezamos a medir la velocidad de nuestra función de potenciación
    if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
        printf("Error: No se ha podido empezar a medir la velocidad de nuestra función de potenciación.\n");
        return -1.0;
    }

    // Calculamos (base elevada a exponent) mod module con nuestra función de potenciación
    get_modular_power(result, base, exponent, module);

    // Terminamos de medir la velocidad de nuestra función de potenciación
    if (clock_gettime(CLOCK_REALTIME, &end) == -1) {
        printf("Error: No se ha podido terminar de medir la velocidad de nuestra función de potenciación.\n");
        return -1.0;
    }

    // Calculamos el tiempo de ejecución de nuestra función de potenciación
    executionTime = get_execution_time(start, end);
    return executionTime;
}

/* Devuelve el tiempo de ejecución de la función mpz_powm de GMP y asigna el resultado en
el mpz_t resultGMP pasado como argumento. */
double get_time_powm_GMP(mpz_t *resultGMP, mpz_t *base, mpz_t *exponent, mpz_t *module) {
    struct timespec start, end;
    double executionTime = 0.0;

    // Control de errores
    if (resultGMP == NULL || base == NULL || exponent == NULL || module == NULL) return -1.0;

    // Empezamos a medir la velocidad de la función mpz_powm de GMP
    if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
        printf("Error: No se ha podido empezar a medir la velocidad de la función mpz_powm de GMP.\n");
        return -1.0;
    }

    /* Devuelve en r el resultado de (base elevada a exponent) mod module. Para usar un exponente negativo
    se requiere de la función mpz_powm_ui y la inversa base^(-1) mod module tiene que existir */
    mpz_powm(*resultGMP, *base, *exponent, *module);

    // Terminamos de medir la velocidad de la función mpz_powm de GMP
    if (clock_gettime(CLOCK_REALTIME, &end) == -1) {
        printf("Error: No se ha podido terminar de medir la velocidad de la función mpz_powm de GMP.\n");
        return -1.0;
    }

    // Calculamos el tiempo de ejecución de la función mpz_powm de GMP
    executionTime = get_execution_time(start, end);
    return executionTime;
}

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./potencia base exponente módulo\n");
    printf("Devuelve por la salida estándar el resultado y tiempo de ejecución de:\n");
    printf("(base elevada a exponente) mod módulo\n");
    printf("---------------------------\n");
}

// Libera la memoria inicializada de las variables mpz
void free_mpz_vars(mpz_t *result, mpz_t *resultGMP, mpz_t *base, mpz_t *exponent, mpz_t *module) {
    if (result != NULL) mpz_clear(*result);
    if (resultGMP != NULL) mpz_clear(*resultGMP);
    if (base != NULL) mpz_clear(*base);
    if (exponent != NULL) mpz_clear(*exponent);
    if (module != NULL) mpz_clear(*module);
}

int main(int argc, char *argv[]) {
    double time = 0.0, timeGMP = 0.0;
    mpz_t result, resultGMP, base, exponent, module;

    // Inicializamos variables
    mpz_init(result);
    mpz_init(resultGMP);
    mpz_init(base);
    mpz_init(exponent);
    mpz_init(module);

    // Comprobar número de argumentos del usuario
    if (argc != 4) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();

        free_mpz_vars(&result, &resultGMP, &base, &exponent, &module);
        return -1;
    }

    // Obtener parámetros del usuario
    // ./potencia base exponente módulo

    // Primer argumento: Base
    if (mpz_set_str(base, argv[1], 10) == -1 || mpz_cmp_si(base, 0) < 0) {
        printf("Error: El valor de la base %s no es válido.\n", argv[1]);

        free_mpz_vars(&result, &resultGMP, &base, &exponent, &module);
        return -1;
    }

    // Segundo argumento: Exponente
    if (mpz_set_str(exponent, argv[2], 10) == -1 || mpz_cmp_si(exponent, 0) < 0) {
        printf("Error: El valor del exponente %s no es válido.\n", argv[2]);

        free_mpz_vars(&result, &resultGMP, &base, &exponent, &module);
        return -1;
    }

    // Tercer argumento: Módulo
    if (mpz_set_str(module, argv[3], 10) == -1 || mpz_cmp_si(module, 0) <= 0) {
        printf("Error: El valor del módulo %s no es válido.\n", argv[3]);

        free_mpz_vars(&result, &resultGMP, &base, &exponent, &module);
        return -1;
    }

    // Calculamos los datos de nuestra función de potenciación
    time = get_time_modular_power(&result, &base, &exponent, &module);
    if (time == -1.0) {
        printf("Error: No se ha podido obtener la información de nuestra función de potenciación.\n");

        free_mpz_vars(&result, &resultGMP, &base, &exponent, &module);
        return -1;
    }

    // Mostramos el resultado de nuestra función de potenciación por la salida estándar
    printf("--- Función propia de potenciación ---\n");
    printf("Tiempo de ejecución: %lfs\n", time);
    gmp_printf("Resultado: %Zd\n", result);

    // Calculamos los datos de la función mpz_powm de GMP
    timeGMP = get_time_powm_GMP(&resultGMP, &base, &exponent, &module);
    if (timeGMP == -1.0) {
        printf("Error: No se ha podido obtener la información de la función mpz_powm de GMP.\n");

        free_mpz_vars(&result, &resultGMP, &base, &exponent, &module);
        return -1;
    }

    // Mostramos el resultado de la función mpz_powm por la salida estándar
    printf("--- Función mpz_powm de GMP ---\n");
    printf("Tiempo de ejecución: %lfs\n", timeGMP);
    gmp_printf("Resultado: %Zd\n", resultGMP);

    // Liberamos variables
    free_mpz_vars(&result, &resultGMP, &base, &exponent, &module);

    return 0;
}

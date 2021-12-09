#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/euclides.h"

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./potencia base exponente módulo\n");
    printf("Devuelve por la salida estándar el resultado de:\n");
    printf("(base elevada a exponente) mod módulo\n");
    printf("---------------------------\n");
}

// Libera la memoria inicializada de las variables mpz
void free_mpz_vars (mpz_t *result, mpz_t *base, mpz_t *exponent, mpz_t *module) {
    if (result != NULL) mpz_clear(*result);
    if (base != NULL) mpz_clear(*base);
    if (exponent != NULL) mpz_clear(*exponent);
    if (module != NULL) mpz_clear(*module);
}

int main (int argc, char *argv[]) {
    mpz_t result, base, exponent, module;

    // Inicializamos variables
    mpz_init(result);
    mpz_init(base);
    mpz_init(exponent);
    mpz_init(module);

    // Comprobar número de argumentos del usuario
    if (argc != 4) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();

        free_mpz_vars(&result, &base, &exponent, &module);
        return -1;
    }

    // Obtener parámetros del usuario
    // ./potencia base exponente módulo

    // Primer argumento: Base
    if (mpz_set_str(base, argv[1], 10) == -1) {
        printf("Error: El valor de la base %s no es válido.\n", argv[1]);

        free_mpz_vars(&result, &base, &exponent, &module);
        return -1;
    }

    // Segundo argumento: Exponente
    if (mpz_set_str(exponent, argv[2], 10) == -1) {
        printf("Error: El valor del exponente %s no es válido.\n", argv[2]);

        free_mpz_vars(&result, &base, &exponent, &module);
        return -1;
    }

    // Tercer argumento: Módulo
    if (mpz_set_str(module, argv[3], 10) == -1) {
        printf("Error: El valor del módulo %s no es válido.\n", argv[3]);

        free_mpz_vars(&result, &base, &exponent, &module);
        return -1;
    }

    /* Devuelve en r el resultado de (base elevada a exponent) mod module. Para usar un exponente negativo
    se requiere de la función mpz_powm_ui y la inversa base^(-1) mod module tiene que existir */
    mpz_powm(result, base, exponent, module);
    gmp_printf("%Zd\n", result);

    /*
    Devuelve un número positivo si op1>op2, negativo si op1<op2 y 0 si op1=op2.
    int mpz_cmp_si(const mpz_t op1, signed long int op2)

    Devuelve en r el resultado de n mod d. El signo del divisor es ignorado y el resultado no puede ser negativo.
    void mpz_mod(mpz_t r, const mpz_t n, const mpz_t d)

    Muestra en pantalla una variable mpz.
    gmp_printf("%Zd", mpz_t)

    Devuelve el valor de la variable mpz en un signed long int. Para comprobar si el valor entra en esos bytes
    se usa la función mpz_fits_slong_p.
    signed long int mpz_get_si(const mpz_t op)
    */

    // Liberamos variables
    free_mpz_vars(&result, &base, &exponent, &module);

    return 0;
}

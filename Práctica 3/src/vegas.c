#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <sys/random.h>

#include "../includes/miller-rabin.h"
#include "../includes/euclides.h"

/**
 * Asigna en r uno de los dos números primos p y q que multiplicados obtienen el módulo
 * de RSA n, mediante el algoritmo de las vegas a partir de m y k. Las variables
 * m y k han sido calculadas con la clave pública (e, n) y el exponente de descifrado d.
 * Recibe un estado aleatorio para generar una base diferente en cada ejecución.
 * 
 * Devuelve 1 si el algoritmo encuentra respuesta, 0 en caso contrario y -1 en caso de error.
 */
int perform_vegas_algorithm(mpz_t *r, mpz_t *m, mpz_t *k, mpz_t *n, gmp_randstate_t randState) {
    int i;
    mpz_t a, top, x, y;

    // Control de errores
    if (r == NULL || m == NULL || k == NULL || n == NULL) return -1;

    // Inicializamos las variables
    mpz_init(a);
    mpz_init(top);
    mpz_init(x);
    mpz_init(y);

    // Asignamos los valores inciales
    mpz_set(top, *n);
    mpz_sub_ui(top, top, 1);

    // Generamos un número entero aleatorio entre 1 y n-1
    mpz_urandomm(a, randState, top);
    mpz_add_ui(a, a, 1);

    // Comprobamos si mcd(a, n) > 1
    calculate_mcd(r, &a, n);
    if (mpz_cmp_si(*r, 1) > 0) {
        return 1; // Respuesta de p o q = mcd(a, n)
    }

    // Hacemos la operación x = a^m mod n
    mpz_powm(x, a, *m, *n);

    // Comprobamos si no hay respuesta (x == 1 | x == n-1)
    if (mpz_cmp_si(x, 1) == 0 || mpz_cmp(x, top) == 0) {
        mpz_clear(a);
        mpz_clear(top);
        mpz_clear(x);
        mpz_clear(y);
        return 0; // No responde
    }

    // De 1 a k-1
    for (i = 1; i < mpz_get_ui(*k); i++) {
        // y = x
        mpz_set(y, x);

        // x = x^2 mod n
        mpz_mul(x, x, x);
        mpz_mod(x, x, *n);

        // x == 1
        if (mpz_cmp_si(x, 1) == 0) {
            // Respuesta de p o q = mcd(y+1, n)
            mpz_add_ui(y, y, 1);
            calculate_mcd(r, &y, n);

            mpz_clear(a);
            mpz_clear(top);
            mpz_clear(x);
            mpz_clear(y);
            return 1;

        // x == -1
        } else if (mpz_cmp_si(x, -1) == 0) {
            mpz_clear(a);
            mpz_clear(top);
            mpz_clear(x);
            mpz_clear(y);
            return 0; // No responde
        }
    }

    // Respuesta de p o q = mcd(x+1, n)
    mpz_add_ui(x, x, 1);
    calculate_mcd(r, &x, n);

    // Liberamos memoria
    mpz_clear(a);
    mpz_clear(top);
    mpz_clear(x);
    mpz_clear(y);

    return 1;
}

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./vegas {-n módulo} {-e exponenteCifrado} {-d exponenteDescifrado}\n");
    printf("Siendo los parámetros:\n");
    printf("-n módulo del RSA resultante de la multiplicación de dos números primos p y q\n");
    printf("-e exponente de cifrado perteneciente a la clave pública de RSA\n");
    printf("-d exponente de descifrado perteneciente a la clave privada de RSA\n");
    printf("---------------------------\n");
}

// Libera la memoria inicializada de las variables mpz
void free_mpz_vars(mpz_t *n, mpz_t *e, mpz_t *d, mpz_t *eByd, mpz_t *m, mpz_t *k, mpz_t *p, mpz_t *q) {
    // Argumentos del usuario
    if (n != NULL) mpz_clear(*n);
    if (e != NULL) mpz_clear(*e);
    if (d != NULL) mpz_clear(*d);

    // Variables necesarias para el algoritmo de las vegas
    if (eByd != NULL) mpz_clear(*eByd);
    if (m != NULL) mpz_clear(*m);
    if (k != NULL) mpz_clear(*k);

    // Resultados del algoritmo de las vegas
    if (p != NULL) mpz_clear(*p);
    if (q != NULL) mpz_clear(*q);
}

int main(int argc, char *argv[]) {
    int i, bits = 0, ret = 0;
    gmp_randstate_t randState;
    unsigned long seed;
    mpz_t n, e, d, eByd, m, k, p, q;

    // Generamos una semilla aleatoria en cada ejecución del programa
    if (getrandom(&seed, sizeof(unsigned long), GRND_NONBLOCK) == -1) {
        printf("Error: No se ha podido generar una semilla aleatoria para esta ejecución del programa.\n");
        return -1;
    }

    // Inicializamos memoria para la semilla generada
    gmp_randinit_mt(randState);
    gmp_randseed_ui(randState, seed);

    // Inicializamos las variables de los argumentos del usuario
    mpz_init(n);
    mpz_init(e);
    mpz_init(d);

    // Inicializamos las variables necesarias para el algoritmo de las vegas
    mpz_init(eByd);
    mpz_init(m);
    mpz_init(k);

    // Inicializamos las variables de los resultados del algoritmo de las vegas
    mpz_init(p);
    mpz_init(q);

    // Comprobar número de argumentos del usuario
    if (argc != 7) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();

        gmp_randclear(randState);
        free_mpz_vars(&n, &e, &d, &eByd, &m, &k, &p, &q);
        return -1;
    }

    // Obtener parámetros del usuario
    // vegas {-n módulo} {-e exponenteCifrado} {-d exponenteDescifrado}
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            // Módulo del RSA resultante de la multiplicación de dos números primos p y q
            if (mpz_set_str(n, argv[++i], 10) == -1) {
                printf("Error: El parámetro -n %s no es válido.\n", argv[i]);

                gmp_randclear(randState);
                free_mpz_vars(&n, &e, &d, &eByd, &m, &k, &p, &q);
                return -1;
            }

        } else if (strcmp(argv[i], "-e") == 0) {
            // Exponente de cifrado perteneciente a la clave pública de RSA
            if (mpz_set_str(e, argv[++i], 10) == -1) {
                printf("Error: El parámetro -e %s no es válido.\n", argv[i]);

                gmp_randclear(randState);
                free_mpz_vars(&n, &e, &d, &eByd, &m, &k, &p, &q);
                return -1;
            }

        } else if (strcmp(argv[i], "-d") == 0) {
            // Exponente de descifrado perteneciente a la clave privada de RSA
            if (mpz_set_str(d, argv[++i], 10) == -1) {
                printf("Error: El parámetro -d %s no es válido.\n", argv[i]);

                gmp_randclear(randState);
                free_mpz_vars(&n, &e, &d, &eByd, &m, &k, &p, &q);
                return -1;
            }

        } else {
            printf("Error: Parámetro incorrecto %s.\n", argv[i]);
            print_help();

            gmp_randclear(randState);
            free_mpz_vars(&n, &e, &d, &eByd, &m, &k, &p, &q);
            return -1;
        }
    }

    // Multiplicamos ambos exponentes e y d para el posterior cálculo de m y k
    mpz_mul(eByd, e, d);

    // Con base 2 devuelve la localización del bit 1 más significativo del valor e*d
    bits = mpz_sizeinbase(eByd, 2);

    // Calculamos las variables m y k necesarias para el algoritmo de las vegas. (e*d)-1 = 2^k * m
    if (calculate_mk(&m, &k, &eByd, bits) == -1) {
        printf("Error: No se han podido calcular las variables m y k necesarias para el algoritmo de las vegas.\n");

        gmp_randclear(randState);
        free_mpz_vars(&n, &e, &d, &eByd, &m, &k, &p, &q);
        return -1;
    }

    printf("Factorizando el módulo del RSA mediante el algoritmo de las vegas...\n");

    // Ejecutamos el algoritmo de las vegas hasta encontrar respuesta
    while (!ret) {
        // Suponemos que, en caso de respuesta, el número primo calculado en el algoritmo es p
        ret = perform_vegas_algorithm(&p, &m, &k, &n, randState);
        if (ret == -1) {
            printf("Error: No se ha podido ejecutar el algoritmo de las vegas.\n");

            gmp_randclear(randState);
            free_mpz_vars(&n, &e, &d, &eByd, &m, &k, &p, &q);
            return -1;
        }
    }
    printf("OK: Módulo del RSA factorizado mediante el algoritmo de las vegas.\n");

    // Una vez obtenido p, hallamos q
    mpz_fdiv_q(q, n, p);

    // Mostramos en pantalla el p y q calculados del algoritmo de las vegas
    gmp_printf("Clave pública: (%Zd, %Zd)\n", e, n);
    gmp_printf("Exponente privado: %Zd\n", d);
    gmp_printf("p: %Zd\n", p);
    gmp_printf("q: %Zd\n", q);

    // Liberamos memoria
    gmp_randclear(randState);
    free_mpz_vars(&n, &e, &d, &eByd, &m, &k, &p, &q);

    return 0;
}

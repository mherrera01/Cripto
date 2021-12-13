#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <sys/random.h>

#include "../includes/randomPrime.h"
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
    printf("./vegas {-b bits}\n");
    printf("Siendo los parámetros:\n");
    printf("-b número máximo de bits que tendrá el módulo n del RSA generado\n");
    printf("---------------------------\n");
}

// Libera la memoria inicializada de las variables mpz
void free_mpz_vars(mpz_t *n, mpz_t *p, mpz_t *q, mpz_t *f, mpz_t *e, mpz_t *d, mpz_t *eByd, mpz_t *m, mpz_t *k) {
    // Inicializamos las variables necesarias para RSA
    if (n != NULL) mpz_clear(*n);
    if (p != NULL) mpz_clear(*p);
    if (q != NULL) mpz_clear(*q);

    if (f != NULL) mpz_clear(*f);
    if (e != NULL) mpz_clear(*e);
    if (d != NULL) mpz_clear(*d);

    // Variables necesarias para el algoritmo de las vegas
    if (eByd != NULL) mpz_clear(*eByd);
    if (m != NULL) mpz_clear(*m);
    if (k != NULL) mpz_clear(*k);
}

int main(int argc, char *argv[]) {
    int i, bits = 0, primeError = 0, generateExp = 1, ret = 0;
    char *convertToLong;
    RandomPrimeInfo *result = NULL;
    gmp_randstate_t randState;
    unsigned long seed;
    mpz_t n, p, q, f, e, d, eByd, m, k, top, aux;

    // Generamos una semilla aleatoria en cada ejecución del programa
    if (getrandom(&seed, sizeof(unsigned long), GRND_NONBLOCK) == -1) {
        printf("Error: No se ha podido generar una semilla aleatoria para esta ejecución del programa.\n");
        return -1;
    }

    // Inicializamos memoria para la semilla generada
    gmp_randinit_mt(randState);
    gmp_randseed_ui(randState, seed);

    // Inicializamos las variables necesarias para RSA
    mpz_init(n);
    mpz_init(p);
    mpz_init(q);

    mpz_init(f);
    mpz_init(e);
    mpz_init(d);

    // Inicializamos las variables necesarias para el algoritmo de las vegas
    mpz_init(eByd);
    mpz_init(m);
    mpz_init(k);

    // Comprobar número de argumentos del usuario
    if (argc != 3) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();

        gmp_randclear(randState);
        free_mpz_vars(&n, &p, &q, &f, &e, &d, &eByd, &m, &k);
        return -1;
    }

    // Obtener parámetros del usuario
    // vegas {-b bits}
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-b") == 0) {
            // Número máximo de bits que tendrá el módulo n del RSA generado
            bits = (int) strtol(argv[++i], &convertToLong, 10);

            // Comprobamos si no se ha convertido ningún caracter o el valor no es válido
            if (argv[i] == convertToLong || bits <= 3) {
                printf("Error: El parámetro -b %s no es válido.\n", argv[i]);

                gmp_randclear(randState);
                free_mpz_vars(&n, &p, &q, &f, &e, &d, &eByd, &m, &k);
                return -1;
            }

        } else {
            printf("Error: Parámetro incorrecto %s.\n", argv[i]);
            print_help();

            gmp_randclear(randState);
            free_mpz_vars(&n, &p, &q, &f, &e, &d, &eByd, &m, &k);
            return -1;
        }
    }

    // Generamos dos números primos aleatorios p y q con la mínima probabilidad de equivocación
    result = generate_random_prime(&p, bits/2, 0.0, randState);
    if (result == NULL) {
        primeError = 1;
    } else free(result);

    result = generate_random_prime(&q, bits/2, 0.0, randState);
    if (result == NULL) {
        primeError = 1;
    } else free(result);

    // Error en la generación de p y q
    if (primeError) {
        printf("Error: No se han podido generar los números primos aleatorios p y q.\n");

        gmp_randclear(randState);
        free_mpz_vars(&n, &p, &q, &f, &e, &d, &eByd, &m, &k);
        return -1;
    }

    // Calculamos el módulo n del RSA resultante de la multiplicación de p y q
    mpz_mul(n, p, q);

    // Obtenemos fi de n con la función de euler
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(f, p, q); // f(n) = (p-1) * (q-1) al ser p y q primos

    // Restablecemos los valores de p y q
    mpz_add_ui(p, p, 1);
    mpz_add_ui(q, q, 1);

    // Número máximo de la generación de aleatorios
    mpz_init(top);
    mpz_sub_ui(top, f, 1);

    mpz_init(aux);

    // Seleccionamos un exponente de cifrado cuyo mcd(e, f(n)) = 1 y 1 < e < f(n)
    while(generateExp) {
        // Generamos un número entero aleatorio entre 1 y f(n)-1
        mpz_urandomm(e, randState, top);
        mpz_add_ui(e, e, 1);

        // Comprobamos si el exponente es válido
        calculate_mcd(&aux, &e, &f);
        if (mpz_cmp_si(aux, 1) == 0) break;
    }

    mpz_clear(top);
    mpz_clear(aux);

    // Calculamos el exponente de descifrado
    get_modular_inverse(&d, &f, &e);

    // Mostramos en pantalla los números generados del RSA
    printf("--- Datos RSA generados ---\n");
    gmp_printf("Exponente público: %Zd\n", e);
    gmp_printf("Módulo: %Zd\n", n);
    gmp_printf("Exponente privado: %Zd\n", d);
    gmp_printf("p: %Zd\n", p);
    gmp_printf("q: %Zd\n", q);

    // Multiplicamos ambos exponentes e y d para el posterior cálculo de m y k
    mpz_mul(eByd, e, d);

    // Calculamos las variables m y k necesarias para el algoritmo de las vegas. (e*d)-1 = 2^k * m
    if (calculate_mk(&m, &k, &eByd, bits) == -1) {
        printf("Error: No se han podido calcular las variables m y k necesarias para el algoritmo de las vegas.\n");

        gmp_randclear(randState);
        free_mpz_vars(&n, &p, &q, &f, &e, &d, &eByd, &m, &k);
        return -1;
    }

    printf("\nFactorizando el módulo del RSA mediante el algoritmo de las vegas...\n");

    // Ejecutamos el algoritmo de las vegas hasta encontrar respuesta
    while (!ret) {
        // Suponemos que, en caso de respuesta, el número primo calculado en el algoritmo es p
        ret = perform_vegas_algorithm(&p, &m, &k, &n, randState);
        if (ret == -1) {
            printf("Error: No se ha podido ejecutar el algoritmo de las vegas.\n");

            gmp_randclear(randState);
            free_mpz_vars(&n, &p, &q, &f, &e, &d, &eByd, &m, &k);
            return -1;
        }
    }
    printf("OK: Módulo del RSA factorizado mediante el algoritmo de las vegas.\n\n");

    // Una vez obtenido p, hallamos q
    mpz_fdiv_q(q, n, p);

    // Mostramos en pantalla el p y q calculados del algoritmo de las vegas
    printf("--- Resultado del algoritmo de las vegas ---\n");
    gmp_printf("p: %Zd\n", p);
    gmp_printf("q: %Zd\n", q);

    // Liberamos memoria
    gmp_randclear(randState);
    free_mpz_vars(&n, &p, &q, &f, &e, &d, &eByd, &m, &k);

    return 0;
}

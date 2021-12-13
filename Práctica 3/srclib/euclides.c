#include <stdio.h>
#include <stdlib.h>
#include "../includes/euclides.h"

void calculate_mcd(mpz_t *r, mpz_t *a, mpz_t *b) {
    mpz_t dividend, divisor;

    // Control de errores
    if (r == NULL || a == NULL || b == NULL) return;

    // Inicializamos las variables
    mpz_init(dividend);
    mpz_init(divisor);

    // Asignamos los valores iniciales
    mpz_set(dividend, *a);
    mpz_set(divisor, *b);

    // Obtenemos el cociente de la división de a y b
    // mpz_fdiv_q()

    // Obtenemos el módulo de la división de a y b
    mpz_mod(*r, dividend, divisor);

    // Liberamos las variables
    mpz_clear(dividend);
    mpz_clear(divisor);

    // Función recursiva
    return;
}

void get_modular_inverse(mpz_t *r, mpz_t *m, mpz_t *n) {
    mpz_t ma, na, x, c[3];
    mpz_t *p = NULL;
    int i, p_size = 3;

    // Control de errores
    if (r == NULL || m == NULL || n == NULL) return;

    // El inverso multiplicativo de 1 siempre es 1
    if (mpz_cmp_si(*n, 1) == 0) {
        mpz_set(*r, *n);
        return;
    }

    // Inicializamos memoria para los resultados de p0, p1 y p2
    p = (mpz_t*) malloc (p_size * sizeof(mpz_t));
    if (p == NULL) return;
    for (i=0; i<p_size; i++) mpz_init(p[i]);

    // Inicializamos las variables
    mpz_init(ma);
    mpz_init(na);
    mpz_init(x);
    for (i=0; i<3; i++) mpz_init(c[i]);

    // Asignamos los valores iniciales
    mpz_set(ma, *m);
    mpz_set(na, *n);

    mpz_set_str(x, "1", 10);
    mpz_set_str(p[1], "1", 10);

    // Seguimos mientras que el resto sea diferente a 0
    for (i=0; mpz_cmp_si(x, 0) != 0; i++) {
        // Obtenemos el resto de la división
        mpz_mod(x, ma, na);

        // Actualizamos las variables con los cocientes de las tres operaciones anteriores
        mpz_set(c[2], c[1]);
        mpz_set(c[1], c[0]);

        // Obtenemos el cociente de la división
        mpz_fdiv_q (c[0], ma, na);

        // Asignamos el dividendo y divisor de la siguiente operación
        mpz_set(ma, na);
        if (mpz_cmp_si(x, 0) != 0) mpz_set(na, x);

        // Actualizamos los resultados de p para obtener el inverso multiplicativo
        if (i>1) {
            mpz_mul(p[i], p[i-1], c[2]);
            mpz_sub(p[i], p[i-2], p[i]);
            mpz_mod(p[i], p[i], *m);

            // Añadimos memoria para la siguiente p
            p_size++;
            p = (mpz_t*) realloc (p, p_size * sizeof(mpz_t));
            mpz_init(p[p_size-1]);
        }
    }

    // Comprobamos si no hay inverso multiplicativo
    if (mpz_cmp_si(na, 1) != 0) {
        mpz_set_str(*r, "0", 10);
    } else {
        // Obtenemos el inverso multiplicativo de la última p
        mpz_mul(p[i], p[i-1], c[1]);
        mpz_sub(p[i], p[i-2], p[i]);
        mpz_mod(p[i], p[i], *m);

        // Asignamos el inverso multiplicativo al resultado
        mpz_set(*r, p[i]);
    }

    // Liberamos las variables
    mpz_clear(ma);
    mpz_clear(na);
    mpz_clear(x);
    for(i=0; i<3; i++) mpz_clear(c[i]);

    // Liberamos memoria
    for (i=0; i<p_size; i++) mpz_clear(p[i]);
    free(p);

    return;
}

int get_modular_intverse(int m, int n) {
    mpz_t mr, mm, mn;
    int ret;

    // Inicializamos las varibales mpz
    mpz_init(mr);
    mpz_init(mm);
    mpz_init(mn);

    // Les damos a las varibales mpz el valor de los ints
    mpz_set_si(mm, m);
    mpz_set_si(mn, n);
    
    // Calculamos el inverso modular
    get_modular_inverse(&mr, &mm, &mn);

    // Convertimos el resultado a int
    ret = (int) mpz_get_si(mr);

    // Borramos las varibales mpz
    mpz_clear(mr);
    mpz_clear(mm);
    mpz_clear(mn);

    return ret;
}

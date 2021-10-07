#ifndef EUCLIDES_H
#define EUCLIDES_H

#include <gmp.h>

/*Calcula el máximo común divisor de dos números mediante el algoritmo de Euclides.*/
mpz_t *calculate_mcd(mpz_t *r, mpz_t *a, mpz_t *b);

/*Devuelve en r el inverso multiplicativo de m en n (aritmética modular) y 0 en caso de no existir.*/
void get_modular_inverse(mpz_t *r, mpz_t *m, mpz_t *n);

#endif

#include <stdio.h>
#include <stdlib.h>
#include "euclides.h"

mpz_t *calculate_mcd(mpz_t *r, mpz_t *a, mpz_t *b){
    if(mpz_cmp_si(*b, 0) == 0) return a;
    if(mpz_cmp_si(*b, 1) == 0) return r;
    mpz_mod(*r, *a, *b);
    //gmp_printf("mod(%Zd,%Zd)=%Zd\n", a, b, r);
    return calculate_mcd(a, b, r);

    //mpz_mod (mpz_t r, const mpz_t n, const mpz_t d)
    //mpz_cmp_si (const mpz_t op1, signed long int op2)
    //mpz_cmp (const mpz_t op1, const mpz_t op2)
}

void get_modular_inverse(mpz_t *r, mpz_t *m, mpz_t *n){
    mpz_t ma, na, x, c[3];
    mpz_t *p = NULL;
    int i, p_size;

    mpz_init(ma);
    mpz_init(na);

    for (i=0; i<3; i++) mpz_init(c[i]);
    mpz_init(x);

    p_size = 50;
    p = (mpz_t*) malloc(p_size*sizeof(mpz_t));

    mpz_set (ma, *m);
    mpz_set (na, *n);

    mpz_set_str(x, "1", 10);

    for(i=0; i<p_size; i++){
        mpz_init(p[i]);
    }

    mpz_set_str(p[1], "1", 10);

    for(i=0; mpz_cmp_si(x, 0) != 0; i++){
        mpz_mod(x, ma, na);
        mpz_set(c[2], c[1]);
        mpz_set(c[1], c[0]);
        mpz_fdiv_q (c[0], ma, na);
        //gmp_printf("%Zd = %Zd(%Zd) + %Zd    \t", ma, c[0], na, x);
        mpz_set(ma, na);
        if(mpz_cmp_si(x, 0) != 0) mpz_set(na, x);
        if(i>1){
            mpz_mul(p[i], p[i-1], c[2]);
            mpz_sub(p[i], p[i-2], p[i]);
            mpz_mod(p[i], p[i], *m);
        }
        /*
        p = realloc(p, ++p_size);
        mpz_init(p[p_size-1]);
        */
        //gmp_printf("p = %Zd - %Zd(%Zd) mod %Zd = %Zd p_size=%d\n",p[i-2], p[i-1], c[2], m, p[i], p_size);
    }

    if(mpz_cmp_si(na, 1) != 0){
        mpz_set_str(*r, "0", 10);
        //printf("No hay inverso multiplicativo\n");
        return;
    }
    //p[i+1] = (p[i-1] - p[i]*c[1])%m
    mpz_mul(p[i], p[i-1], c[1]);
    mpz_sub(p[i], p[i-2], p[i]);
    mpz_mod(p[i], p[i], *m);

    //gmp_printf("p = %Zd - %Zd(%Zd) mod %Zd = %Zd\n",p[i-2], p[i-1], c[1], m, p[i]);
    mpz_set(*r, p[i]);

    mpz_clear(ma);
    mpz_clear(na);
    mpz_clear(x);
    for(i=0; i<2; i++) mpz_clear(c[i]);
    for (i=0; i<p_size; i++) mpz_clear(p[i]);
    free(p);

    return;
}

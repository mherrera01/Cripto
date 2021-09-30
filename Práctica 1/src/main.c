#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

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

/*
get_inverse(n, m){
    ma = m
    na = n
    for(i = 0; x!=0; i++){
        x=ma%na
        c[2]=c[1]
        c[1]=c[0]
        c[0]=ma/na
        p[i] = (p[i-2] - p[i-1]*c[2])%m
    }
    if(na!=1) fail;
    p[i+1] = (p[i-1] - p[i]*c[1])%m
}
*/

void get_modular_inverse(mpz_t *r, mpz_t *m, mpz_t *n){
    mpz_t ma, na, x, c[3];
    mpz_t *p = NULL;
    int i;

    mpz_init(ma);
    mpz_init(na);
    mpz_init(c[0]);
    mpz_init(c[1]);
    mpz_init(c[2]);
    mpz_init(x);
    p = (mpz_t*) malloc(200*sizeof(mpz_t));

    mpz_set (ma, *m);
    mpz_set (na, *n);

    mpz_set_str(x, "1", 10);

    for(i=0; i<200; i++){
        mpz_init(p[i]);
    }

    mpz_set_str(p[1], "1", 10);

    for(i=0; mpz_cmp_si(x, 0) != 0; i++){
        mpz_mod(x, ma, na);
        mpz_set(c[2], c[1]);
        mpz_set(c[1], c[0]);
        mpz_fdiv_q (c[0], ma, na);
        gmp_printf("%Zd = %Zd(%Zd) + %Zd    ", ma, c[0], na, x);
        mpz_set(ma, na);
        if(mpz_cmp_si(x, 0) != 0) mpz_set(na, x);
        mpz_mul(p[i], p[i-1], c[2]);
        mpz_sub(p[i], p[i-2], p[i]);
        mpz_mod(p[i], p[i], *m);
        gmp_printf("p=%Zd\n", p[i]);
    }

    if(mpz_cmp_si(na, 1) != 0){
        mpz_set_str(*r, "0", 10);
        printf("Arriba España!\n");
        return;
    }
    //p[i+1] = (p[i-1] - p[i]*c[1])%m
    mpz_mul(p[i+1], p[i], c[1]);
    mpz_sub(p[i+1], p[i-1], p[i+1]);
    mpz_mod(p[i+1], p[i+1], *m);

    gmp_printf("p[i+1] es %Zd\n", p[i+1]);
    mpz_set(*r, p[i+1]);
    return;
}

int main (int argc,char *argv[]) {
    mpz_t a,b,r;
    char sa[100], sb[100];

    mpz_init(a);
    mpz_init(b);
    mpz_init(r);
    mpz_set_str(r,"1",10);

    printf("Introduce un valor para a:\n");
    scanf("%s", sa);
    mpz_set_str (a,sa,10);
    printf("Introduce un valor para b:\n");
    scanf("%s", sb);
    mpz_set_str (b,sb,10);

    get_modular_inverse(&r, &a, &b);

    gmp_printf("El inverso multiplicativo de %s y %s es %Zd\n", sa, sb, r);

    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(r);

    return 0;
}

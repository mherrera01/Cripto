#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

mpz_t *calculate_mcd(mpz_t *r, mpz_t *a, mpz_t *b){
    if(mpz_cmp_si(*b, 0) == 0) return a;
    if(mpz_cmp_si(*b, 1) == 0) return r;
    mpz_mod(*r, *a, *b);
    return calculate_mcd(a, b, r);

    //mpz_mod (mpz_t r, const mpz_t n, const mpz_t d)
    //mpz_cmp_si (const mpz_t op1, signed long int op2)
    //mpz_cmp (const mpz_t op1, const mpz_t op2)
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

    gmp_printf("El mcd de %s y %s es %Zd\n", sa, sb, calculate_mcd(&r, &a, &b));

    return 0;
}

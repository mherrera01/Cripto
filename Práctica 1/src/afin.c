#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <string.h>
#include "euclides.h"

char affine_char(char origin, int a, int b, int n){
    printf("%d*%d+%d mod %d = %d\n", (origin-65), a, b, n, (((origin-65)*a+b)%n));
    return (((origin-65)*a+b)%n)+65;
}

//get_modular_inverse(mpz_t *r, mpz_t *m, mpz_t *n);
void affine_cypher(char* message, mpz_t* inv, mpz_t* a, mpz_t* b, mpz_t* n){
    int i;
    long la, lb, ln;

    // Convertimos la a y la b a positivo si estos están en negativo
    if (mpz_cmp_si(*a, 0) == -1){
        mpz_mod(*a, *a, *n);
    }
    
    if (mpz_cmp_si(*b, 0) == -1){
        mpz_mod(*b, *b, *n);
    }

    get_modular_inverse(inv, a, n);

    if(mpz_cmp_si(*inv, 0) == 0){
        gmp_printf("%Zd no tiene inverso multiplicativo.\n", a);
        return;
    }

    la =  mpz_get_si(*a);
    lb =  mpz_get_si(*b);
    ln =  mpz_get_si(*n);

    for(i=0; i<strlen(message); i++){
        message[i] = affine_char(message[i], la, lb, ln);
    }

    //message[strlen(message)]=0;
}

int main (int argc,char *argv[]) {
    mpz_t a, b, n, inv;
    char sa[100], sb[100], message[100];

    mpz_init(a);
    mpz_init(b);
    mpz_init(n);
    mpz_init(inv);
    mpz_set_str(n, "26", 10);

    
    printf("Introduce un valor para a:\n");
    scanf("%s", sa);
    mpz_set_str (a,sa,10);
    printf("Introduce un valor para b:\n");
    scanf("%s", sb);
    mpz_set_str (b,sb,10);
    printf("Introduce un mensaje:\n");
    scanf("%s", message);
    
    /*
    mpz_set_str (a,"26",10);
    mpz_set_str (b,"15",10);
    */

    affine_cypher(message, &inv, &a, &b, &n);

    printf("El mensaje cifrado es %s\n", message);

    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(n);
    mpz_clear(inv);

    return 0;
}

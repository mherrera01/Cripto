#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <string.h>
#include "../includes/alphabet.h"
#include "../includes/euclides.h"

#define MAX_MESSAGE_SIZE 100

char affine_encrypt_char(char origin, int a, int b, int n) {
    int result;

    origin = get_letter_code(origin);
    if (origin == -1) return '_'; // Caracter desconocido

    result = ((origin)*a+b)%n;
    return get_letter(result);
}

char affine_decrypt_char(char origin, int inv, int b, int n) {
    int result;

    origin = get_letter_code(origin);
    if (origin == -1) return '_'; // Caracter desconocido

    result = ((origin-b)*inv)%n;
    return get_letter(result<0?result+n:result); // Si el resultado es negativo, obtenemos su equivalente positivo.
}

void affine_encrypt(char* message, mpz_t* inv, mpz_t* a, mpz_t* b, mpz_t* n){
    int i;
    long la, lb, ln;

    // Control de errores
    if (message == NULL || inv == NULL || a == NULL || b == NULL || n == NULL) return;

    // Convertimos la a y la b a positivo si estos están en negativo
    if (mpz_cmp_si(*a, 0) == -1) {
        mpz_mod(*a, *a, *n);
    }
    
    if (mpz_cmp_si(*b, 0) == -1) {
        mpz_mod(*b, *b, *n);
    }

    get_modular_inverse(inv, n, a);

    if (mpz_cmp_si(*inv, 0) == 0) {
        gmp_printf("%Zd no tiene inverso multiplicativo.\n", a);
        return;
    }

    la = mpz_get_si(*a);
    lb = mpz_get_si(*b);
    ln = mpz_get_si(*n);

    for (i=0; i<strlen(message); i++){
        message[i] = affine_encrypt_char(message[i], la, lb, ln);
    }
}

void affine_decrypt(char* message, mpz_t* inv, mpz_t* a, mpz_t* b, mpz_t* n){
    int i;
    long linv, lb, ln;

    // Control de errores
    if (message == NULL || inv == NULL || a == NULL || b == NULL || n == NULL) return;

    // Convertimos la a y la b a positivo si estos están en negativo
    if (mpz_cmp_si(*a, 0) == -1) {
        mpz_mod(*a, *a, *n);
    }
    
    if (mpz_cmp_si(*b, 0) == -1) {
        mpz_mod(*b, *b, *n);
    }

    get_modular_inverse(inv, n, a);

    if (mpz_cmp_si(*inv, 0) == 0) {
        gmp_printf("%Zd no tiene inverso multiplicativo.\n", a);
        return;
    }

    linv =  mpz_get_si(*inv);
    lb =  mpz_get_si(*b);
    ln =  mpz_get_si(*n);

    for (i=0; i<strlen(message); i++) {
        message[i] = affine_decrypt_char(message[i], linv, lb, ln);
    }
}

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./afin {-C|-D} {-m |Zm|} {-a Nx} {-b N+} [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-C el programa cifra\n");
    printf("-D el programa descifra\n");
    printf("-m tamaño del espacio de texto cifrado\n");
    printf("-a coeficiente multiplicativo de la función afín\n");
    printf("-b término constante de la función afín\n");
    printf("-i fichero de entrada\n");
    printf("-o fichero de salida\n");
    printf("---------------------------\n");
}

// Libera la memoria inicializada de las variables mpz
void free_mpz_vars (mpz_t *a, mpz_t *b, mpz_t *n, mpz_t *inv) {
    if (a != NULL) mpz_clear(*a);
    if (b != NULL) mpz_clear(*b);
    if (n != NULL) mpz_clear(*n);
    if (inv != NULL) mpz_clear(*inv);
}

// Cerramos los archivos de entrada y salida abiertos previamente
void close_files (FILE *input, FILE *output) {
    if (input == NULL || output == NULL) return;

    // Cerramos los archivos si no son stdin/stdout
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);
}

int main (int argc, char *argv[]) {
    mpz_t a, b, n, inv;
    char message[MAX_MESSAGE_SIZE];
    int i, modo = 0; // modo en 0 para cifrar y 1 para descifrar
    FILE *input = stdin, *output = stdout;

    // Inicializamos variables
    mpz_init(a);
    mpz_init(b);
    mpz_init(n);
    mpz_init(inv);

    // Comprobar número de argumentos del usuario
    if (argc < 8 || argc > 12) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();

        free_mpz_vars(&a, &b, &n, &inv);
        return -1;
    }

    // Obtener parámetros del usuario
    // afin {-C|-D} {-m |Zm|} {-a Nx} {-b N+} [-i filein] [-o fileout]
    for (i = 1; i < argc; i++) {
        if (strcmp("-C", argv[i]) == 0) {
            // Cifrar
            modo = 0;

        } else if (strcmp("-D", argv[i]) == 0) {
            // Descifrar
            modo = 1;

        } else if (strcmp(argv[i], "-m") == 0) {
            // Tamaño del espacio de texto cifrado
            if (mpz_set_str(n, argv[++i], 10) == -1) {
                printf("Error: El parámetro -m %s no es válido.\n", argv[i]);

                close_files(input, output);
                free_mpz_vars(&a, &b, &n, &inv);
                return -1;
            }

        } else if (strcmp(argv[i], "-a") == 0) {
            // Coeficiente multiplicativo de la función afín
            if (mpz_set_str(a, argv[++i], 10) == -1) {
                printf("Error: El parámetro -a %s no es válido.\n", argv[i]);

                close_files(input, output);
                free_mpz_vars(&a, &b, &n, &inv);
                return -1;
            }

        } else if (strcmp(argv[i], "-b") == 0) {
            // Término constante de la función afín
            if (mpz_set_str(b, argv[++i], 10) == -1) {
                printf("Error: El parámetro -b %s no es válido.\n", argv[i]);

                close_files(input, output);
                free_mpz_vars(&a, &b, &n, &inv);
                return -1;
            }

        // Parámetros de ficheros entrada/salida. Por defecto stdin y stdout
        } else if (strcmp(argv[i], "-i") == 0) {
            // Abrimos fichero de entrada
            input = fopen(argv[++i], "r");
            if (input == NULL) {
                printf("Error: El archivo de entrada %s no se ha podido encontrar.\n", argv[i]);

                close_files(input, output);
                free_mpz_vars(&a, &b, &n, &inv);
                return -1;
            }

        } else if (strcmp(argv[i], "-o") == 0) {
            // Abrimos fichero de salida
            output = fopen(argv[++i], "w");
            if (output == NULL) {
                printf("Error: El archivo de salida %s no se ha podido encontrar.\n", argv[i]);

                close_files(input, output);
                free_mpz_vars(&a, &b, &n, &inv);
                return -1;
            }

        } else {
            printf("Error: Parámetro incorrecto %s.\n", argv[i]);
            print_help();

            close_files(input, output);
            free_mpz_vars(&a, &b, &n, &inv);
            return -1;
        }
    }

    // Cargamos el alfabeto del archivo alphabet/alphabet.txt
    if (load_alphabet(mpz_get_si(n)) == -1) {
        printf("Error: El alfabeto no se pudo cargar.\n");

        close_files(input, output);
        free_mpz_vars(&a, &b, &n, &inv);
        return -1;
    }
    
    // Leemos el mensaje a cifrar/descifrar
    fgets(message, MAX_MESSAGE_SIZE, input);
    message[strlen(message)-1] = 0;

    if (modo == 0) {
        // Ciframos el mensaje
        affine_encrypt(message, &inv, &a, &b, &n);

        // Mostramos el mensaje cifrado
        fprintf(output, "El mensaje cifrado es %s\n", message);
    } else {
        // Desciframos el mensaje
        affine_decrypt(message, &inv, &a, &b, &n);

        // Mostramos el mensaje descifrado
        fprintf(output, "El mensaje descifrado es %s\n", message);
    }

    // Liberamos alfabeto
    destroy_alphabet();

    // Cerramos los archivos si no son stdin/stdout
    close_files(input, output);

    // Liberamos variables
    free_mpz_vars(&a, &b, &n, &inv);

    return 0;
}

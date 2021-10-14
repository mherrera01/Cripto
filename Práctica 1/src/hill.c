#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <string.h>
#include "../includes/alphabet.h"
#include "../includes/matrix.h"

#define MAX_MESSAGE_SIZE 100

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./hill {-C|-D} {-m |Zm|} {-n Nk} {-k filek} [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-C el programa cifra\n");
    printf("-D el programa descifra\n");
    printf("-m tamaño del espacio de texto cifrado\n");
    printf("-n dimensión de la matriz de transformación\n");
    printf("-k fichero que contiene la matriz de transformación\n");
    printf("-i fichero de entrada\n");
    printf("-o fichero de salida\n");
    printf("---------------------------\n");
}

// Libera la memoria inicializada de las variables mpz
void free_mpz_vars (mpz_t *n, mpz_t *k, mpz_t *m) {
    if (n != NULL) mpz_clear(*n);
    if (k != NULL) mpz_clear(*k);
    if (m != NULL) mpz_clear(*m);
}

// Cerramos los archivos de entrada y salida abiertos previamente
void close_files (FILE *input, FILE *output) {
    if (input == NULL || output == NULL) return;

    // Cerramos los archivos si no son stdin/stdout
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);
}

int main (int argc, char *argv[]) {
    mpz_t n, k, m;
    char message[MAX_MESSAGE_SIZE];
    int i, modo = 0; // modo en 0 para cifrar y 1 para descifrar
    FILE *input = stdin, *output = stdout;

    // Inicializamos variables
    mpz_init(n);
    mpz_init(k);
    mpz_init(m);

    // Comprobar número de argumentos del usuario
    if (argc < 8 || argc > 12) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();

        free_mpz_vars(&n, &k, &m);
        return -1;
    }

    // Obtener parámetros del usuario
    // hill {-C|-D} {-m |Zm|} {-n Nk} {-k filek} [-i filein] [-o fileout]
    for (i = 1; i < argc; i++) {
        if (strcmp("-C", argv[i]) == 0) {
            // Cifrar
            modo = 0;

        } else if (strcmp("-D", argv[i]) == 0) {
            // Descifrar
            modo = 1;

        } else if (strcmp(argv[i], "-m") == 0) {
            // Tamaño del espacio de texto cifrado
            if (mpz_set_str(m, argv[++i], 10) == -1) {
                printf("Error: El parámetro -m %s no es válido.\n", argv[i]);

                close_files(input, output);
                free_mpz_vars(&n, &k, &m);
                return -1;
            }

        } else if (strcmp(argv[i], "-n") == 0) {
            // Dimensión de la matriz de transformación
            if (mpz_set_str(n, argv[++i], 10) == -1) {
                printf("Error: El parámetro -n %s no es válido.\n", argv[i]);

                close_files(input, output);
                free_mpz_vars(&n, &k, &m);
                return -1;
            }

        } else if (strcmp(argv[i], "-k") == 0) {
            // Fichero que contiene la matriz de transformación
            if (mpz_set_str(k, argv[++i], 10) == -1) {
                printf("Error: El parámetro -k %s no es válido.\n", argv[i]);

                close_files(input, output);
                free_mpz_vars(&n, &k, &m);
                return -1;
            }

        // Parámetros de ficheros entrada/salida. Por defecto stdin y stdout
        } else if (strcmp(argv[i], "-i") == 0) {
            // Abrimos fichero de entrada
            input = fopen(argv[++i], "r");
            if (input == NULL) {
                printf("Error: El archivo %s no se ha podido encontrar.\n", argv[i]);

                close_files(input, output);
                free_mpz_vars(&n, &k, &m);
                return -1;
            }

        } else if (strcmp(argv[i], "-o") == 0) {
            // Abrimos fichero de salida
            output = fopen(argv[++i], "w");
            if (output == NULL) {
                printf("Error: El archivo %s no se ha podido encontrar.\n", argv[i]);

                close_files(input, output);
                free_mpz_vars(&n, &k, &m);
                return -1;
            }

        } else {
            printf("Error: Parámetro incorrecto %s.\n", argv[i]);
            print_help();

            close_files(input, output);
            free_mpz_vars(&n, &k, &m);
            return -1;
        }
    }

    // Cargamos el alfabeto del archivo alphabet/alphabet.txt
    if (load_alphabet(mpz_get_si(m)) == -1) {
        printf("Error: El alfabeto no se pudo cargar.\n");

        close_files(input, output);
        free_mpz_vars(&n, &k, &m);
        return -1;
    }

    // Liberamos alfabeto
    destroy_alphabet();

    // Cerramos los archivos si no son stdin/stdout
    close_files(input, output);

    // Liberamos variables
    free_mpz_vars(&n, &k, &m);

    return 0;
}

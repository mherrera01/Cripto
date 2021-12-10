#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./primo {-b bits} {-p sec} [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-b número máximo de bits significativos que tendrá el número generado\n");
    printf("-p probabilidad de equivocación del algoritmo que especifica la seguridad que debería tener el primo generado en el test\n");
    printf("-o fichero de salida\n");
    printf("---------------------------\n");
}

// Cerramos el archivo de salida abierto previamente
void close_file(FILE *output) {
    if (output == NULL) return;

    // Cerramos el archivo si no es stdout
    if (output != stdout) fclose(output);
}

int main(int argc, char *argv[]) {
    mpz_t prime;
    int i, bits = 0, sec = 0;
    char *convertToLong;
    FILE *output = stdout;

    // Inicializamos la variable con el número primo resultante
    mpz_init(prime);

    // Comprobar número de argumentos del usuario
    if (argc < 5 || argc > 7) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();

        mpz_clear(prime);
        return -1;
    }

    // Obtener parámetros del usuario
    // primo {-b bits} {-p sec} [-o fileout]
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-b") == 0) {
            // Número máximo de bits significativos que tendrá el número generado
            bits = (int) strtol(argv[++i], &convertToLong, 10);

            // Comprobamos si no se ha convertido ningún caracter
            if (argv[i] == convertToLong) {
                printf("Error: El parámetro -b %s no es válido.\n", argv[i]);

                close_file(output);
                mpz_clear(prime);
                return -1;
            }

        } else if (strcmp(argv[i], "-p") == 0) {
            // Probabilidad de equivocación del algoritmo que especifica la seguridad que debería tener el primo generado en el test
            sec = (int) strtol(argv[++i], &convertToLong, 10);

            // Comprobamos si no se ha convertido ningún caracter
            if (argv[i] == convertToLong) {
                printf("Error: El parámetro -p %s no es válido.\n", argv[i]);

                close_file(output);
                mpz_clear(prime);
                return -1;
            }

        // Parámetro de fichero de salida. Por defecto stdout
        } else if (strcmp(argv[i], "-o") == 0) {
            // Abrimos fichero de salida
            output = fopen(argv[++i], "w");
            if (output == NULL) {
                printf("Error: El archivo de salida %s no se ha podido encontrar.\n", argv[i]);

                close_file(output);
                mpz_clear(prime);
                return -1;
            }

        } else {
            printf("Error: Parámetro incorrecto %s.\n", argv[i]);
            print_help();

            close_file(output);
            mpz_clear(prime);
            return -1;
        }
    }

    // Cerramos el archivo si no es stdout
    close_file(output);

    // Liberamos la variable con el número primo resultante
    mpz_clear(prime);

    return 0;
}

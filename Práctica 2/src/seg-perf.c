#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./seg-perf {-P|-I} [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-P se utiliza el método equiprobable.\n");
    printf("-I se utiliza el método no equiprobable.\n");
    printf("-i fichero de entrada\n");
    printf("-o fichero de salida\n");
    printf("---------------------------\n");
}

// Cerramos los archivos de entrada y salida abiertos previamente
void close_files (FILE *input, FILE *output) {
    if (input == NULL || output == NULL) return;

    // Cerramos los archivos si no son stdin/stdout
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);
}

int main (int argc, char *argv[]) {
    int i;
    int modo = 0; // modo en 0 para método equiprobable y 1 para no equiprobable
    FILE *input = stdin, *output = stdout;

    // Comprobar número de argumentos del usuario
    if (argc < 2 || argc > 6) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();
        return -1;
    }

    // Obtener parámetros del usuario
    // seg-perf {-P|-I} [-i filein] [-o fileout]
    for (i = 1; i < argc; i++) {
        if (strcmp("-P", argv[i]) == 0) {
            // Método equiprobable
            modo = 0;

        } else if (strcmp("-I", argv[i]) == 0) {
            // Método no equiprobable
            modo = 1;

        // Parámetros de ficheros entrada/salida. Por defecto stdin y stdout
        } else if (strcmp(argv[i], "-i") == 0) {
            // Abrimos fichero de entrada
            input = fopen(argv[++i], "r");
            if (input == NULL) {
                printf("Error: El archivo de entrada %s no se ha podido encontrar.\n", argv[i]);

                close_files(input, output);
                return -1;
            }

        } else if (strcmp(argv[i], "-o") == 0) {
            // Abrimos fichero de salida
            output = fopen(argv[++i], "w");
            if (output == NULL) {
                printf("Error: El archivo de salida %s no se ha podido encontrar.\n", argv[i]);

                close_files(input, output);
                return -1;
            }

        } else {
            printf("Error: Parámetro incorrecto %s.\n", argv[i]);
            print_help();

            close_files(input, output);
            return -1;
        }
    }

    // Cerramos los archivos si no son stdin/stdout
    close_files(input, output);

    return 0;
}

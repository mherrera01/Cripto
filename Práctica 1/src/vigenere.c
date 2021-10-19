#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/alphabet.h"

int vigenere_encrypt() {
    return 0;
}

int vigenere_decrypt() {
    return 0;
}

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./vigenere {-C|-D} {-k clave} [-m |Zm|] [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-C el programa cifra\n");
    printf("-D el programa descifra\n");
    printf("-k cadena de caracteres usada como clave\n");
    printf("-m tamaño del espacio de texto cifrado\n");
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
    char *k, *convertToLong, message[100]; // TODO
    int i, keyLength = 0, m = 26, modo = 0; // modo en 0 para cifrar y 1 para descifrar
    FILE *input = stdin, *output = stdout;

    // Comprobar número de argumentos del usuario
    if (argc < 4 || argc > 8) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();
        return -1;
    }

    // Obtener parámetros del usuario
    // vigenere {-C|-D} {-k clave} [-i filein] [-o fileout]
    for (i = 1; i < argc; i++) {
        if (strcmp("-C", argv[i]) == 0) {
            // Cifrar
            modo = 0;

        } else if (strcmp("-D", argv[i]) == 0) {
            // Descifrar
            modo = 1;

        } else if (strcmp(argv[i], "-k") == 0) {
            // Cadena de caracteres usada como clave
            k = argv[++i];
        
        // Parámetro adicional
        } else if (strcmp(argv[i], "-m") == 0) {
            // Tamaño del espacio de texto cifrado
            m = (int) strtol(argv[++i], &convertToLong, 10);

            // Comprobamos si no se ha convertido ningún caracter
            if (argv[i] == convertToLong) {
                printf("Error: El parámetro -m %s no es válido.\n", argv[i]);

                close_files(input, output);
                return -1;
            }

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

    // Cargamos el alfabeto del archivo alphabet/alphabet.txt
    if (load_alphabet(m) == -1) {
        printf("Error: El alfabeto no se pudo cargar.\n");

        close_files(input, output);
        return -1;
    }

    // Obtenemos la longitud de la clave para cifrar en grupos
    keyLength = strlen(k);
    if (keyLength <= 0) {
        printf("Error: La clave no es válida.\n");

        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    while (fgets(message, keyLength, input) != NULL) {
        printf("%s\n", message);
    }

    // Leemos el mensaje a cifrar/descifrar
    fgets(message, 100, input); // TODO
    message[strlen(message)-1] = 0;

    if (modo == 0) {
        // Ciframos el mensaje
        if (vigenere_encrypt() == -1) {
            printf("Error: No se ha podido cifrar el mensaje %s con la matriz:\n", message);

            destroy_alphabet();
            close_files(input, output);
            return -1;
        }

        // Mostramos el mensaje cifrado
        fprintf(output, "El mensaje cifrado es %s\n", message); // TODO
    } else {
        // Desciframos el mensaje
        if (vigenere_decrypt() == -1) {
            printf("Error: No se ha podido descifrar el mensaje %s\n", message);

            destroy_alphabet();
            close_files(input, output);
            return -1;
        }

        // Mostramos el mensaje descifrado
        fprintf(output, "El mensaje descifrado es %s\n", message); // TODO
    }

    // Liberamos alfabeto
    destroy_alphabet();

    // Cerramos los archivos si no son stdin/stdout
    close_files(input, output);

    return 0;
}

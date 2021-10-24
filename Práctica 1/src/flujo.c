#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/alphabet.h"

#define MAX_MESSAGE_SIZE 100

int generateRandomNumber(int seed, int m){
    static int generatedNumbers = 0;

    srand(seed+generatedNumbers);
    generatedNumbers++;

    return rand()%m;
}

char encrypt_char(char original, int key) {
    return get_letter_code(original) ^ key;
}

char decrypt_char(char original, int key) {
    return get_letter(original ^ key);
}

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./flujo {-C|-D} {-k clave} [-m |Zm|] [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-C el programa cifra\n");
    printf("-D el programa descifra\n");
    printf("-k número usado como clave\n");
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
    int i, endRead = 0, readChars = 0, k = 0, m = DEFAULT_ALPHABET_SIZE;
    FILE *input = stdin, *output = stdout;
    int modo = 0; // modo en 0 para cifrar y 1 para descifrar
    char *convertToLong, *message = NULL;

    // Comprobar número de argumentos del usuario
    if (argc < 4 || argc > 8) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();
        return -1;
    }

    // Obtener parámetros del usuario
    // flujo {-C|-D} {-k clave} [-m |Zm|] [-i filein] [-o fileout]
    for (i = 1; i < argc; i++) {
        if (strcmp("-C", argv[i]) == 0) {
            // Cifrar
            modo = 0;

        } else if (strcmp("-D", argv[i]) == 0) {
            // Descifrar
            modo = 1;

        } else if (strcmp(argv[i], "-k") == 0) {
            // Número usado como clave.
            k = (int) strtol(argv[++i], &convertToLong, 10);
            
            // Comprobamos si no se ha convertido ningún caracter.
            if (argv[i] == convertToLong) {
                printf("Error: El parámetro -k %s no es válido.\n", argv[i]);

                close_files(input, output);
                return -1;
            }

        // Parámetro adicional
        } else if (strcmp(argv[i], "-m") == 0) {
            // Tamaño del espacio de texto cifrado.
            m = (int) strtol(argv[++i], &convertToLong, 10);

            // Comprobamos si no se ha convertido ningún caracter.
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
    if ((m = load_alphabet(m)) == -1) {
        printf("Error: El alfabeto no se pudo cargar.\n");

        close_files(input, output);
        return -1;
    }

    // Inicializamos el buffer del mensaje a leer
    message = (char *) malloc ((MAX_MESSAGE_SIZE + 1) * sizeof(char));
    if (message == NULL) {
        printf("Error: No se ha podido inicializar la memoria del mensaje a leer.\n");

        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    // Leemos el mensaje a cifrar/descifrar
    while (1) {
        // Leemos en stdin con fgets para terminar en un salto de línea
        if (input == stdin) {
            if (fgets(message, (MAX_MESSAGE_SIZE + 1), input) == NULL) break;

            // En el salto de línea dejamos de leer
            endRead = (strchr(message, '\n') != NULL);
            if (endRead) message[strcspn(message, "\n")] = '\0';

            readChars = strlen(message);

        // Leemos archivos binarios con fread
        } else {
            readChars = fread(message, sizeof(char), MAX_MESSAGE_SIZE, input);
            if (readChars <= 0) break;
        }

        // Ciframos caracter a caracter con el cifrado de flujo
        for (i = 0; i < readChars; i++) {
            if (modo == 0) {
                fprintf(output, "%c", encrypt_char(message[i], generateRandomNumber(k, m)));
            } else {
                fprintf(output, "%c", decrypt_char(message[i], generateRandomNumber(k, m)));
            }
        }

        // Limpiamos el buffer del mensaje
        memset(message, 0, MAX_MESSAGE_SIZE + 1);

        // Salir del bucle en stdin
        if (endRead) break;
    }

    if (output == stdout) fprintf(output, "\n");

    // Liberamos memoria
    free(message);

    // Liberamos alfabeto
    destroy_alphabet();

    // Cerramos los archivos si no son stdin/stdout
    close_files(input, output);

    return 0;
}

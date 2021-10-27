#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/coincidencia.h"
#include "../includes/alphabet.h"

#define MAX_KEY_SIZE 100
#define MAX_TEXT_SIZE 1000000

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./IC [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
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

int main(int argc, char *argv[]) {
    char cipherText[MAX_TEXT_SIZE] = {0}, **subStrings = NULL;
    int i, keyLength, m, keyLengthFound = 0;
    double averageDeviationENG, averageDeviationESP;
    FILE *input = stdin, *output = stdout;

    // Comprobar número de argumentos del usuario
    if (argc < 1 || argc > 5) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();
        return -1;
    }

    // Obtener parámetros del usuario
    // IC [-i filein] [-o fileout]
    for (i = 1; i < argc; i++) {
        // Parámetros de ficheros entrada/salida. Por defecto stdin y stdout
        if (strcmp(argv[i], "-i") == 0) {
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
    m = load_alphabet("alphabet/alphabet.txt", COINCIDENCE_ALPHABET_SIZE);
    if (m == -1 || m != COINCIDENCE_ALPHABET_SIZE) {
        // El alfabeto debe coincidir con el de la tabla de frecuencias
        printf("Error: El alfabeto debe coincidir con la tabla de frecuencias, conteniendo todas las letras ");
        printf("en mayúsculas de A-Z (tamaño %d).\n", COINCIDENCE_ALPHABET_SIZE);

        close_files(input, output);
        return -1;
    }

    if (output == stdout) fprintf(output, "Leyendo mensaje cifrado...\n");

    // Leemos el mensaje cifrado al que averiguar la clave 
    if (fgets(cipherText, MAX_TEXT_SIZE, input) == NULL) {
        // Error al leer el mensaje
        if (ferror(input)) {
            printf("Error: No se ha podido leer el mensaje del archivo.\n");

            destroy_alphabet();
            close_files(input, output);
            return -1;
        }
    }

    if (output == stdout) fprintf(output, "OK: Mensaje cifrado leído.\n");
    if (output == stdout) fprintf(output, "Calculando tamaño de la clave...\n");

    // Calculamos mediante el índice de coincidencia el tamaño de la clave
    for (keyLength = 1; keyLength < MAX_KEY_SIZE; keyLength++) {
        averageDeviationENG = 0.0;
        averageDeviationESP = 0.0;

        // Dividimos la cadena de texto por el tamaño de la clave
        subStrings = divide_strings(cipherText, keyLength);
        if (subStrings == NULL) {
            printf("Error: No se ha podido dividir la cadena de texto por el tamaño de clave %d.\n", keyLength);

            destroy_alphabet();
            close_files(input, output);
            return -1;
        }

        // Calculamos el índice de coincidencia con una clave en específico
        for (i = 0; i < keyLength; i++) {
            averageDeviationENG += check_key_length_IC(subStrings[i], keyLength, 0); // Inglés
            averageDeviationESP += check_key_length_IC(subStrings[i], keyLength, 1); // Castellano
        }

        // Comprobamos si el tamaño de la clave es el correcto
        if (averageDeviationENG/keyLength >= 0.063 || averageDeviationESP/keyLength >= 0.075) {
            keyLengthFound = 1;
            break;
        }

        // Liberamos memoria
        free_substrings(subStrings, keyLength);
    }

    // Comprobamos si el tamaño de la clave ha sido encontrado
    if (!keyLengthFound) {
        printf("Error: No se ha podido encontrar un tamaño válido para la clave.\n");

        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    // Tamaño de clave encontrado
    if (output == stdout) fprintf(output, "OK: El tamaño de la clave es %d.\n", keyLength);
    if (output == stdout) fprintf(output, "La clave es: ");

    // Mostramos en pantalla (o en el archivo de outuput) la clave obtenida letra a letra para cada subgrupo.
    for (i = 0; i < keyLength; i++) {
        fprintf(output, "%c", get_caesar_key(subStrings[i]));
    }

    if (output == stdout) fprintf(output, "\n");

    // Liberamos memoria
    free_substrings(subStrings, keyLength);

    // Liberamos alfabeto
    destroy_alphabet();

    // Cerramos los archivos si no son stdin/stdout
    close_files(input, output);

    return 0;
}

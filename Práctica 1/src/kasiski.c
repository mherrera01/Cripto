#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/coincidencia.h"
#include "../includes/alphabet.h"

#define MINIMUM_COINCIDENCE_SIZE 6
#define MAX_KEY_SIZE 100
#define MAX_TEXT_SIZE 1000000

int calculate_mcd(int a, int b) {
    if (b == 0) return a;
    if (b == 1) return 1;
    return calculate_mcd(b, a%b); // Obtenemos el módulo de la división de a y b
}

char* get_repeated_string(char* cipherText) {
    int i, j, k, len, coincidence;
    char* keyword = NULL;

    // Control de errores
    if (cipherText == NULL) return NULL;

    // Obtenemos la longitud del texto cifrado
    len = strlen(cipherText);
    if (len <= 2*MINIMUM_COINCIDENCE_SIZE) {
        return NULL;
    }

    // Reservamos memoria para la palabra repetida
    keyword = (char *) calloc (MINIMUM_COINCIDENCE_SIZE + 1, sizeof(char));
    if (keyword == NULL) return NULL;

    for (i = MINIMUM_COINCIDENCE_SIZE; i < len; i++) {
        coincidence = 0;
        for (j = 0; j < len-MINIMUM_COINCIDENCE_SIZE; j++) {
            if (cipherText[j] == cipherText[j+i]) {
                coincidence++;
                if (coincidence == MINIMUM_COINCIDENCE_SIZE) {
                    for (k = j - MINIMUM_COINCIDENCE_SIZE; k < j; k++) {
                        keyword[k+MINIMUM_COINCIDENCE_SIZE-j] = cipherText[k+1];
                    }
                    break;
                }
            } else {
                coincidence = 0;
            }
        }
    }

    return keyword;
}

int *get_index_in_string(char* string, char* substring, int *wordIndexSize) {
    int substringLength, i, coincidencia = 0, *wordIndex = NULL;

    // Control de errores
    if (string == NULL || substring == NULL || wordIndexSize == NULL) return NULL;

    // Obtenemos la longitud de la subcadena de texto
    substringLength = strlen(substring);
    if (substringLength <= 0) return NULL;

    *wordIndexSize = 0;

    // Reservamos memoria para la lista de índices de la subcadena de texto
    wordIndex = (int *) malloc (*wordIndexSize * sizeof(int));
    if (wordIndex == NULL) return NULL;

    // Iteramos letra por letra viendo si coinciden con la subcadena y si coinciden vamos sumando 1 al contador coincidencia
    // hasta llegar a tener tantas letras coincidentes de seguido como la longitud de la subcadena. Entonces guardamos el
    // índice en la variable wordIndex.
    for (i = 0; i < strlen(string); i++) {
        if (string[i] == substring[coincidencia]) {
            coincidencia++;
        } else {
            coincidencia = 0;
        }

        // Se ha encontrado la subcadena de texto
        if (coincidencia == substringLength) {
            *wordIndexSize += 1;

            // Ajustamos el tamaño de la memoria al número de índices
            wordIndex = (int *) realloc (wordIndex, *wordIndexSize * sizeof(int));
            if (wordIndex == NULL) return NULL;

            wordIndex[*wordIndexSize-1] = i-substringLength;
            coincidencia = 0;
        }
    }

    return wordIndex;
}

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./kasiski [-i filein] [-o fileout]\n");
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
    char cipherText[MAX_TEXT_SIZE] = {0}, *repeatedString = NULL;
    int i, mcd, m, *indexes = NULL, indexesSize;
    FILE *input = stdin, *output = stdout;

    // Comprobar número de argumentos del usuario
    if (argc < 1 || argc > 5) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();
        return -1;
    }

    // Obtener parámetros del usuario
    // kasiski [-i filein] [-o fileout]
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

    repeatedString = get_repeated_string(cipherText);
    if (repeatedString == NULL) {
        return -1;
    }

    printf("%s\n", repeatedString);

    indexes = get_index_in_string(cipherText, repeatedString, &indexesSize);
    if (indexes == NULL) {
        free(repeatedString);
        return -1;
    }

    if (indexesSize < 3) {
        free(repeatedString);
        free(indexes);
        return -1;
    }

    for (i = 1; i < indexesSize; i++){
        indexes[i] -= indexes[0];
    }

    mcd = indexes[1];

    for (i = 2; i < indexesSize; i++){
        mcd = calculate_mcd(indexes[i], mcd);
    }

    printf("MCD: %d\n", mcd);

    // Liberamos memoria
    free(indexes);
    free(repeatedString);

    return 0;
}

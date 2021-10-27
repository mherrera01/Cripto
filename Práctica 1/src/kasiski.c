#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/coincidencia.h"
#include "../includes/alphabet.h"

#define MINIMUM_COINCIDENCE_SIZE 3
#define MAX_TEXT_SIZE 1000000

int calculate_mcd(int a, int b) {
    if (b == 0) return a;
    if (b == 1) return 1;
    return calculate_mcd(b, a%b); // Obtenemos el módulo de la división de a y b
}

char* get_repeated_string(char* cipherText, int coincidenceSize) {
    int i, j, k, x, len, coincidence, nWords = 0, inList;
    char* keyword = NULL;

    // Control de errores
    if (cipherText == NULL || coincidenceSize < MINIMUM_COINCIDENCE_SIZE) return NULL;

    // Obtenemos la longitud del texto cifrado
    len = strlen(cipherText);
    if (len <= 2*coincidenceSize) {
        return NULL;
    }

    // Lista de palabras repetidas
    char repeatedWords[100][coincidenceSize];

    // Reservamos memoria para la palabra repetida
    keyword = (char *) calloc (coincidenceSize + 1, sizeof(char));
    if (keyword == NULL) return NULL;

    for (i = coincidenceSize; i < len; i++) {
        coincidence = 0;
        for (j = 0; j < len-coincidenceSize; j++) {
            if (cipherText[j] == cipherText[j+i]) {
                coincidence++;
                if (coincidence == coincidenceSize) {
                    // Guardamos la palabra repetida
                    for (k = j - coincidenceSize; k < j; k++) {
                        keyword[k+coincidenceSize-j] = cipherText[k+1];
                        repeatedWords[nWords][k+coincidenceSize-j] = cipherText[k+1];
                    }

                    // Comprobamos si la palabra ya está en la lista
                    for (k = 0; k < nWords; k++) {
                        inList = 0;
                        for (x = 0; x < coincidenceSize; x++) {
                            if (repeatedWords[k][x] == keyword[x]) {
                                inList++;
                            }
                        }
                        // La palabra está en la lista por lo que al menos se ha repetido tres veces
                        if (inList == coincidenceSize) return keyword;
                    }

                    nWords++;
                }
            } else {
                coincidence = 0;
            }
        }
    }

    // No se ha encontrado la palabra repetida al menos tres veces
    return NULL;
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
    printf("./kasiski [-s coincidenceSize] [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-s longitud de la cadena que se quiere buscar repetida\n");
    printf("-i fichero de entrada\n");
    printf("-o fichero de salida\n");
    printf("---------------------------\n");
}

// Libera la memoria inicializada
void free_mem (char *repeatedString, int *indexes) {
    if (repeatedString != NULL) free(repeatedString);
    if (indexes != NULL) free(indexes);
}

// Cerramos los archivos de entrada y salida abiertos previamente
void close_files (FILE *input, FILE *output) {
    if (input == NULL || output == NULL) return;

    // Cerramos los archivos si no son stdin/stdout
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);
}

int main(int argc, char *argv[]) {
    char cipherText[MAX_TEXT_SIZE] = {0}, *convertToLong, *repeatedString = NULL, **subStrings = NULL;
    int i, keyLength, m, *indexes = NULL, indexesSize, coincidenceSize = MINIMUM_COINCIDENCE_SIZE;
    FILE *input = stdin, *output = stdout;

    // Comprobar número de argumentos del usuario
    if (argc < 1 || argc > 7) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();
        return -1;
    }

    // Obtener parámetros del usuario
    // kasiski [-s coincidenceSize] [-i filein] [-o fileout]
    for (i = 1; i < argc; i++) {
        // Parámetro adicional
        if (strcmp(argv[i], "-s") == 0) {
            // Longitud de la cadena que se quiere buscar repetida
            coincidenceSize = (int) strtol(argv[++i], &convertToLong, 10);

            // Comprobamos si no se ha convertido ningún caracter
            if (argv[i] == convertToLong) {
                printf("Error: El parámetro -s %s no es válido.\n", argv[i]);

                close_files(input, output);
                return -1;
            }

            // Comprobamos si la longitud de la cadena es demasiado corta
            if (coincidenceSize < MINIMUM_COINCIDENCE_SIZE) {
                printf("Error: La longitud de la cadena que se quiere buscar repetida no puede ser menor que %d.\n", MINIMUM_COINCIDENCE_SIZE);

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

    // Obtenemos una subcadena de texto que se repite en el texto cifrado
    repeatedString = get_repeated_string(cipherText, coincidenceSize);
    if (repeatedString == NULL) {
        printf("Error: No se ha podido obtener una subcadena de texto que se repite en el texto cifrado.\n");

        free_mem(repeatedString, indexes);
        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    // Obtenemos los índices donde se encuentra la subcadena de texto repetida
    indexes = get_index_in_string(cipherText, repeatedString, &indexesSize);

    // Comprobamos si hay suficientes repeticiones de la subcadena para hacer kasiski o ha habido algún error
    if (indexes == NULL || indexesSize < 3) {
        printf("Error: No se han podido obtener los índices de la subcadena de texto repetida.\n");

        free_mem(repeatedString, indexes);
        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    // Calculamos la diferencia entre los índices
    for (i = 1; i < indexesSize; i++){
        indexes[i] -= indexes[0];
    }

    // Calculamos el máximo común divisor de los índices que será el tamaño de la clave
    keyLength = indexes[1];
    for (i = 2; i < indexesSize; i++){
        keyLength = calculate_mcd(indexes[i], keyLength);
    }

    // Tamaño de clave encontrado
    if (output == stdout) fprintf(output, "OK: El tamaño de la clave es %d.\n", keyLength);

    // Dividimos la cadena de texto por el tamaño de la clave
    subStrings = divide_strings(cipherText, keyLength);
    if (subStrings == NULL) {
        printf("Error: No se ha podido dividir la cadena de texto por el tamaño de clave %d.\n", keyLength);

        free_mem(repeatedString, indexes);
        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    if (output == stdout) fprintf(output, "La clave es: ");

    // Mostramos en el output la clave obtenida letra a letra para cada subgrupo
    for (i = 0; i < keyLength; i++) {
        fprintf(output, "%c", get_caesar_key(subStrings[i]));
    }

    if (output == stdout) fprintf(output, "\n");

    // Liberamos memoria
    free_substrings(subStrings, keyLength);
    free_mem(repeatedString, indexes);

    // Liberamos alfabeto
    destroy_alphabet();

    // Cerramos los archivos si no son stdin/stdout
    close_files(input, output);

    return 0;
}

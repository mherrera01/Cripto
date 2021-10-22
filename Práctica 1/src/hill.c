#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/alphabet.h"
#include "../includes/matrix.h"
#include "../includes/padding.h"

#define MAX_MESSAGE_SIZE 100

char* hill_encrypt(char* message, Matrix* key, int m) {
    Matrix *inverse = NULL;
    int i, j, blockSize;
    char* encryptedMessage = NULL;
    int* block = NULL, * encryptedBlock = NULL;

    // Revisamos que los inputs tengan la memoria reservada.
    if (message == NULL || key == NULL) return NULL;

    // Comprobamos si la matriz tiene inversa
    inverse = calculate_inverse(key, m);
    if (inverse == NULL) {
        printf("Error: La matriz no tiene inversa.\n");
        return NULL;
    } else destroy_matrix(inverse);

    // Obtenemos el tamaño de la matriz, que va a ser el tamaño del bloque también.
    blockSize = get_matrix_size(key);
    if (blockSize == -1) {
        free(encryptedMessage);
        return NULL;
    }

    //if(pad(message, blockSize)) return NULL;

    // Reservamos espacio para el mensaje cifrado.
    encryptedMessage = (char*) calloc (strlen(message)+1, sizeof(char));
    if (encryptedMessage == NULL) return NULL;

    // Reservamos el espacio para cada bloque de caracteres que vamos a cifrar.
    block = (int*) malloc (blockSize*sizeof(int));
    if (block == NULL) {
        free(encryptedMessage);
        return NULL;
    }

    // Iteramos por bloques y dentro de cada bloque por sus caracteres.
    for (i = 0; i < strlen(message)/blockSize; i++) {
        for (j=i*blockSize; j<(i+1)*blockSize; j++) {
            // Guardamos en el bloque el próximo grupo de letras del mensaje.
            block[j-i*blockSize] = get_letter_code(message[j]);
        }

        encryptedBlock = process_block(block, key, m);
        if (encryptedBlock == NULL) {
            free(block);
            free(encryptedMessage);
            return NULL;
        }

        for (j=i*blockSize; j<(i+1)*blockSize; j++) {
            // Guardamos en el mensaje encriptado el resultado de cada bloque.
            encryptedMessage[j] = get_letter(encryptedBlock[j-i*blockSize]);
        }

        // Liberamos la memoria del bloque cifrado
        free(encryptedBlock);
        encryptedBlock = NULL;
    }

    // Liberamos memoria
    free(block);

    return encryptedMessage;
}

char* hill_decrypt(char* message, Matrix* key, int m) {
    Matrix* inverse = NULL;
    char* decryptedMessage = NULL;
    
    // Calculamos la inversa de la matriz, que vamos a usar como clave para descifrar.
    inverse = calculate_inverse(key, m);
    if (inverse == NULL) return NULL;

    // Descifrar es lo mismo que cifrar con la matriz inversa, así que este es el método que utilizaremos.
    decryptedMessage = hill_encrypt(message, inverse, m);

    // Liberamos la matriz inversa
    destroy_matrix(inverse);
    return decryptedMessage;
}

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

// Cerramos los archivos en disco abiertos previamente
void close_files (FILE *k, FILE *input, FILE *output) {
    if (input == NULL || output == NULL) return;

    // Cerramos el archivo con la matriz de transformación
    if (k != NULL) fclose(k);

    // Cerramos los archivos si no son stdin/stdout
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);
}

int main (int argc, char *argv[]) {
    int m = 0, n = 0;
    char *convertToLong, *convertedMsg = NULL, message[MAX_MESSAGE_SIZE];
    int i, modo = 0; // modo en 0 para cifrar y 1 para descifrar
    FILE *k = NULL, *input = stdin, *output = stdout;
    Matrix *matr = NULL;

    // Comprobar número de argumentos del usuario
    if (argc < 8 || argc > 12) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();
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
            m = (int) strtol(argv[++i], &convertToLong, 10);

            // Comprobamos si no se ha convertido ningún caracter
            if (argv[i] == convertToLong) {
                printf("Error: El parámetro -m %s no es válido.\n", argv[i]);

                close_files(k, input, output);
                return -1;
            }

        } else if (strcmp(argv[i], "-n") == 0) {
            // Dimensión de la matriz de transformación
            n = (int) strtol(argv[++i], &convertToLong, 10);

            // Comprobamos si no se ha convertido ningún caracter
            if (argv[i] == convertToLong) {
                printf("Error: El parámetro -n %s no es válido.\n", argv[i]);

                close_files(k, input, output);
                return -1;
            }

        } else if (strcmp(argv[i], "-k") == 0) {
            // Fichero que contiene la matriz de transformación
            k = fopen(argv[++i], "r");
            if (k == NULL) {
                printf("Error: El archivo %s con la matriz de transformación no se ha podido encontrar.\n", argv[i]);

                close_files(k, input, output);
                return -1;
            }

        // Parámetros de ficheros entrada/salida. Por defecto stdin y stdout
        } else if (strcmp(argv[i], "-i") == 0) {
            // Abrimos fichero de entrada
            input = fopen(argv[++i], "r");
            if (input == NULL) {
                printf("Error: El archivo de entrada %s no se ha podido encontrar.\n", argv[i]);

                close_files(k, input, output);
                return -1;
            }

        } else if (strcmp(argv[i], "-o") == 0) {
            // Abrimos fichero de salida
            output = fopen(argv[++i], "w");
            if (output == NULL) {
                printf("Error: El archivo de salida %s no se ha podido encontrar.\n", argv[i]);

                close_files(k, input, output);
                return -1;
            }

        } else {
            printf("Error: Parámetro incorrecto %s.\n", argv[i]);
            print_help();

            close_files(k, input, output);
            return -1;
        }
    }

    // Cargamos el alfabeto del archivo alphabet/alphabet.txt
    if ((m = load_alphabet(m)) == -1) {
        printf("Error: El alfabeto no se pudo cargar.\n");

        close_files(k, input, output);
        return -1;
    }

    // Leemos la matriz de transformación
    matr = read_matrix(k, n, m);
    if (matr == NULL) {
        printf("Error: La matriz con tamaño %d no se pudo inicializar.\n", n);
        printf("El formato del archivo con la matriz es el siguiente:\n");
        printf("1|2|3\n4|5|6\n7|8|9\n");
        printf("Para cualquier tamaño de matriz, siempre que sea cuadrada.\n");

        destroy_alphabet();
        close_files(k, input, output);
        return -1;
    }

    // Leemos el mensaje a cifrar/descifrar
    fgets(message, MAX_MESSAGE_SIZE, input);
    message[strlen(message)-1] = 0;

    if (modo == 0) {
        // Ciframos el mensaje
        convertedMsg = hill_encrypt(message, matr, m);
        if (convertedMsg == NULL) {
            printf("Error: No se ha podido cifrar el mensaje %s con la matriz:\n", message);
            print_matrix(matr);

            destroy_matrix(matr);
            destroy_alphabet();
            close_files(k, input, output);
            return -1;
        }

        // Mostramos el mensaje cifrado
        fprintf(output, "El mensaje cifrado es %s\n", convertedMsg);
    } else {
        // Desciframos el mensaje
        convertedMsg = hill_decrypt(message, matr, m);
        if (convertedMsg == NULL) {
            printf("Error: No se ha podido descifrar el mensaje %s con la matriz:\n", message);
            print_matrix(matr);

            destroy_matrix(matr);
            destroy_alphabet();
            close_files(k, input, output);
            return -1;
        }

        // Mostramos el mensaje descifrado
        fprintf(output, "El mensaje descifrado es %s\n", convertedMsg);
    }

    // Liberamos el mensaje convertido
    free(convertedMsg);

    // Liberamos la matriz
    destroy_matrix(matr);

    // Liberamos alfabeto
    destroy_alphabet();

    // Cerramos los archivos en disco abiertos
    close_files(k, input, output);

    return 0;
}

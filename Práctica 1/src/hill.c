#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/alphabet.h"
#include "../includes/matrix.h"
#include "../includes/padding.h"

char* hill_encrypt(char* message, Matrix* key, int m) {
    int i, invalidBlock = 0, blockSize = 0;
    char *encryptedMessage = NULL;
    int *block = NULL, *encryptedBlock = NULL;

    // Control de errores
    if (message == NULL || key == NULL || m <= 0) return NULL;

    // Obtenemos el tamaño de la matriz, que va a ser el tamaño del bloque también.
    blockSize = get_matrix_size(key);
    if (blockSize == -1 || strlen(message) > blockSize) return NULL;

    // Añadimos padding al bloque para que tenga el tamaño de la matriz
    if (pad(message, blockSize) == -1) return NULL;

    // Reservamos espacio para el mensaje cifrado.
    encryptedMessage = (char*) calloc (strlen(message)+1, sizeof(char));
    if (encryptedMessage == NULL) return NULL;

    // Reservamos el espacio para cada bloque de caracteres que vamos a cifrar.
    block = (int*) malloc (blockSize*sizeof(int));
    if (block == NULL) {
        free(encryptedMessage);
        return NULL;
    }

    // Iteramos por los caracteres dentro del bloque y le asignamos su código en el alfabeto
    for (i = 0; i < strlen(message); i++) {
        block[i] = get_letter_code(message[i]);

        // En caso de que sea un caracter inválido, ponemos _ en todo el bloque
        if (block[i] == -1) {
            invalidBlock = 1;
            break;
        }
    }

    // Ciframos el bloque con la matriz clave
    if (!invalidBlock) {
        encryptedBlock = process_block(block, key, m);
        if (encryptedBlock == NULL) {
            free(block);
            free(encryptedMessage);
            return NULL;
        }
    }

    // Asignamos las letras cifradas al buffer
    for (i = 0; i < strlen(message); i++) {
        if (invalidBlock) {
            encryptedMessage[i] = '_'; // Ha habido algún caracter inválido en el bloque
        } else {
            encryptedMessage[i] = get_letter(encryptedBlock[i]);
        }
    }

    // Liberamos memoria
    free(encryptedBlock);
    free(block);

    return encryptedMessage;
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

// Libera la memoria inicializada
void free_mem (Matrix *matr, Matrix *inverse, char *message) {
    if (matr != NULL) destroy_matrix(matr);
    if (inverse != NULL) destroy_matrix(inverse);
    if (message != NULL) free(message);
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
    int i, endRead = 0, endReadStdin = 0, readChars = 0, blockSize = 0, m = 0, n = 0;
    char *convertToLong, *convertedMsg = NULL, *message = NULL;
    int modo = 0; // modo en 0 para cifrar y 1 para descifrar
    FILE *k = NULL, *input = stdin, *output = stdout;
    Matrix *matr = NULL, *inverse = NULL;

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

    // Comprobamos si la matriz tiene inversa
    inverse = calculate_inverse(matr, m);
    if (inverse == NULL) {
        printf("Error: La matriz clave no tiene inversa.\n");

        free_mem(matr, inverse, message);
        destroy_alphabet();
        close_files(k, input, output);
        return -1;
    }

    // Obtenemos el tamaño de la matriz, que va a ser el tamaño del bloque también
    blockSize = get_matrix_size(matr);
    if (blockSize == -1) {
        printf("Error: No se ha podido obtener el tamaño del bloque.\n");

        free_mem(matr, inverse, message);
        destroy_alphabet();
        close_files(k, input, output);
        return -1;
    }

    // Inicializamos el buffer del mensaje a leer con el tamaño de la matriz
    message = (char *) calloc ((blockSize + 1), sizeof(char));
    if (message == NULL) {
        printf("Error: No se ha podido inicializar la memoria del mensaje a leer.\n");

        free_mem(matr, inverse, message);
        destroy_alphabet();
        close_files(k, input, output);
        return -1;
    }

    // Leemos el mensaje a cifrar/descifrar
    while (!endRead && !endReadStdin) {
        if (fgets(message + readChars, (blockSize - readChars + 1), input) == NULL) {
            // Error al leer el mensaje
            if (ferror(input)) {
                printf("Error: No se ha podido leer el mensaje del archivo.\n");

                free_mem(matr, inverse, message);
                destroy_alphabet();
                close_files(k, input, output);
                return -1;
            } else {
                // End of File
                endRead = 1;

                // Si no queda ningún mensaje en el buffer
                if (message[0] == '\0') break;
            }
        }

        // Si la entrada es stdin en el salto de línea dejamos de leer
        endReadStdin = (input == stdin) && (strchr(message, '\n') != NULL);
        if (endReadStdin) message[strcspn(message, "\n")] = '\0';

        /* Como fgets deja de leer cuando encuentra un salto de línea, continuamos
        leyendo hasta rellenar el bloque del mensaje si la entrada no es stdin */
        if (!(input == stdin) && strchr(message, '\n') != NULL) {
            readChars = strcspn(message, "\n") + 1; // Asignamos por donde debemos seguir leyendo
            message[readChars - 1] = ' '; // Sustituimos el salto de línea por un espacio
            continue;
        } else readChars = 0;

        if (modo == 0) {
            // Ciframos el mensaje
            convertedMsg = hill_encrypt(message, matr, m);
            if (convertedMsg == NULL) {
                printf("Error: No se ha podido cifrar el mensaje %s con la matriz:\n", message);
                print_matrix(matr);

                free_mem(matr, inverse, message);
                destroy_alphabet();
                close_files(k, input, output);
                return -1;
            }

        } else {
            // Descifrar es lo mismo que cifrar con la matriz inversa, así que este es el método que utilizaremos.
            convertedMsg = hill_encrypt(message, inverse, m);
            if (convertedMsg == NULL) {
                printf("Error: No se ha podido descifrar el mensaje %s con la matriz inversa:\n", message);
                print_matrix(inverse);

                free_mem(matr, inverse, message);
                destroy_alphabet();
                close_files(k, input, output);
                return -1;
            }
        }

        // Mostramos el mensaje convertido
        fprintf(output, "%s", convertedMsg);

        // Liberamos la memoria del mensaje convertido
        free(convertedMsg);

        // Limpiamos el buffer del mensaje
        memset(message, 0, blockSize + 1);
    }

    if (output == stdout) fprintf(output, "\n");

    // Liberamos memoria
    free_mem(matr, inverse, message);

    // Liberamos alfabeto
    destroy_alphabet();

    // Cerramos los archivos en disco abiertos
    close_files(k, input, output);

    return 0;
}

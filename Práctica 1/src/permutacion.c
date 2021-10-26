#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/alphabet.h"
#include "../includes/matrixMN.h"
#include "../includes/padding.h"

int yPermutate(MatrixMN* original, MatrixMN* permutated, int* key_y) {
    int i, j;

    // Control de errores
    if (!original || !permutated || !key_y) {
        return -1;
    }

    for (i = 0; i < get_matrixMN_sizeM(original); i++) {
        // Iteramos por las filas para permutarlas por las posiciones en key_y
        for (j = 0; j < get_matrixMN_sizeN(original); j++) {
            if (set_matrixMN_cell(permutated, j, key_y[i], get_matrixMN_cell(original, j, i)) == -1) {
                return -1;
            }
        }
    }

    return 0;
}

int xPermutate(MatrixMN* original, MatrixMN* permutated, int* key_x) {
    int i, j;

    // Control de errores
    if (!original || !permutated || !key_x) {
        return -1;
    }

    for (i = 0; i < get_matrixMN_sizeN(original); i++) {
        // Iteramos por las columnas para permutarlas por las posiciones en key_x
        for (j = 0; j < get_matrixMN_sizeM(original); j++) {
            if (set_matrixMN_cell(permutated, key_x[i], j, get_matrixMN_cell(original, i, j)) == -1) {
                return -1;
            }
        }
    }

    return 0;
}

MatrixMN* permutate_encrypt(MatrixMN* matrix, int* key_y, int* key_x) {
    MatrixMN* halfPermutated = NULL;
    MatrixMN* permutated = NULL;

    // Control de errores
    if (!matrix || !key_x || !key_y) {
        return NULL;
    }

    // Inicializamos la matriz con la permutación por fila
    halfPermutated = init_matrixMN(get_matrixMN_sizeM(matrix), get_matrixMN_sizeN(matrix));
    if (halfPermutated == NULL) return NULL;

    // Permutamos por fila la matriz con la clave dada
    if (yPermutate(matrix, halfPermutated, key_y) == -1) {
        destroy_matrixMN(halfPermutated);
        return NULL;
    }

    // Inicializamos la matriz con el texto cifrado
    permutated = init_matrixMN(get_matrixMN_sizeM(matrix), get_matrixMN_sizeN(matrix));
    if (permutated == NULL) {
        destroy_matrixMN(halfPermutated);
        return NULL;
    }

    // Permutamos por columna la matriz con la clave dada
    if (xPermutate(halfPermutated, permutated, key_x) == -1) {
        destroy_matrixMN(halfPermutated);
        destroy_matrixMN(permutated);
        return NULL;
    }

    // Liberamos memoria
    destroy_matrixMN(halfPermutated);

    return permutated;
}

int decrypt_keys(int* key_y, int* key_x, int size_y, int size_x) {
    int i, *key_y2 = NULL, *key_x2 = NULL;

    // Control de errores
    if (key_y == NULL || key_x == NULL || size_y <= 0 || size_x <= 0) return -1;

    // Creamos dos arrays auxiliares para guardar las claves temporalmente.
    key_y2 = (int*) malloc (size_y * sizeof(int));
    key_x2 = (int*) malloc (size_x * sizeof(int));

    if (key_y2 == NULL || key_x2 == NULL) return -1;

    // Calculamos la clave que invertiría la permutación cifrada.
    for (i = 0; i < size_y; i++) key_y2[key_y[i]] = i;
    for (i = 0; i < size_x; i++) key_x2[key_x[i]] = i;

    // Cargamos los valores de los arrays auxiliares al array original.
    for (i = 0; i < size_y; i++) key_y[i] = key_y2[i];
    for (i = 0; i < size_x; i++) key_x[i] = key_x2[i];

    // Liberamos los arrays auxiliares.
    free(key_y2);
    free(key_x2);

    return 0;
}

MatrixMN* load_text_to_matrix(char* block, int size_y, int size_x) {
    MatrixMN* matrix = NULL;
    int i, j;

    // Control de errores
    if (!block || size_y < 1 || size_x < 1 || size_y * size_x != strlen(block)) {
        return NULL;
    }

    // Inicializamos la matriz MxN resultante del texto
    matrix = init_matrixMN(size_y, size_x);
    if (matrix == NULL) {
        return NULL;
    }

    // Asignamos el texto a los elementos de la matriz MxN
    for (i = 0; i < size_y; i++) {
        for (j = 0; j < size_x; j++) {
            if (set_matrixMN_cell(matrix, j, i, get_letter_code(block[i*size_x+j])) == -1) {
                return NULL;
            }
        }
    }

    return matrix;
}

char* load_matrix_to_text(MatrixMN* matrix) {
    char* block = NULL;
    int i, j, size_y, size_x;

    // Control de errores
    if (!matrix) {
        return NULL;
    }

    // Obtenemos el número de filas y columnas de la matriz
    size_y = get_matrixMN_sizeM(matrix);
    if (size_y == -1) return NULL;

    size_x = get_matrixMN_sizeN(matrix);
    if (size_x == -1) return NULL;

    // Reservamos memoria para el texto
    block = (char*) calloc ((size_x * size_y + 1), sizeof(char));
    if (block == NULL) {
        return NULL;
    }

    // Obtenemos el texto de la matriz
    for (i = 0; i < size_y; i++) {
        for (j = 0; j < size_x; j++) {
            block[i*size_x+j] = get_letter(get_matrixMN_cell(matrix, j, i));
        }
    }

    return block;
}

/**
 * Guarda en un array de enteros una cadena de números separados por comas.
 * Comprueba que están contenidos todos los valores de 0 a N-1 sin repetir en
 * el array y guarda en la variable keySize la longitud del mismo.
 * 
 * Devuelve la clave si ésta es correcta o NULL en caso contrario.
 */
int *get_key(char *k, int *keySize) {
    char *convertToLong, *value = NULL;
    int i, j, valueInt, keyLength = 0, *key = NULL, totalKeyValue = 0;

    // Control de errores
    if (k == NULL || keySize == NULL) return NULL;

    // Obtenemos la longitud de la clave con comas
    keyLength = strlen(k);
    if (keyLength <= 0) return NULL;

    // Obtenemos el primer elemento de la clave
    value = strtok(k, ",");
    if (value == NULL && keyLength != 1) return NULL;

    // Convertimos el valor del primer elemento a Int
    valueInt = (int) strtol(value, &convertToLong, 10);

    // Comprobamos si no se ha convertido ningún caracter
    if (value == convertToLong) return NULL;

    // Los elementos no pueden ser negativos
    if (valueInt < 0) return NULL;

    // Inicializamos la memoria de la clave
    key = (int *) malloc (keyLength * sizeof(int));
    if (key == NULL) return NULL;

    key[0] = valueInt; // Asignamos el primer elemento

    // Obtenemos la longitud de la clave sin comas y asignamos los valores a un array
    for (keyLength = 1; (value = strtok(NULL, ",")) != NULL; keyLength++) {
        if (value == NULL) {
            free(key);
            return NULL;
        }

        // Convertimos el valor del elemento a int
        valueInt = (int) strtol(value, &convertToLong, 10);

        // Comprobamos si no se ha convertido ningún caracter
        if (value == convertToLong) {
            free(key);
            return NULL;
        }

        // Los elementos no pueden ser negativos
        if (valueInt < 0) return NULL;

        key[keyLength] = valueInt; // Asignamos el elemento
    }

    // Ajustamos el tamaño de la memoria a los elementos de la clave sin comas
    key = (int *) realloc (key, keyLength * sizeof(int));
    if (key == NULL) return NULL;

    // Asignamos la longitud de la clave
    *keySize = keyLength;
    
    // Revisamos que no haya ningún valor repetido y que los valores que hay
    // en el array van de 0 a N-1, sin que falte ninguno.
    for (i = 0; i < keyLength; i++) {
        for (j = i + 1; j < keyLength; j++) {
            if (key[i] == key[j]) {
                free(key);
                return NULL; 
            }
        }
        totalKeyValue += (key[i] - i);
    }

    // Comprobamos que, en efecto, los valores que hay van de 0 a N-1.
    if (totalKeyValue != 0){ 
        free(key);
        return NULL;
    }

    return key;
}

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./permutacion {-C|-D} {-k1 K1 -k2 K2} [-m |Zm|] [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-C el programa cifra\n");
    printf("-D el programa descifra\n");
    printf("-k1 vector de números para permutar las filas de P (ej: 0,3,2,1)\n");
    printf("-k2 vector de números para permutar las columnas de P (ej: 1,4,0,2,3)\n");
    printf("-m tamaño del espacio de texto cifrado\n");
    printf("-i fichero de entrada\n");
    printf("-o fichero de salida\n");
    printf("---------------------------\n");
}

// Libera la memoria inicializada
void free_mem (int *key_y, int *key_x, char *message) {
    if (key_y != NULL) free(key_y);
    if (key_x != NULL) free(key_x);
    if (message != NULL) free(message);
}

// Cerramos los archivos de entrada y salida abiertos previamente
void close_files (FILE *input, FILE *output) {
    if (input == NULL || output == NULL) return;

    // Cerramos los archivos si no son stdin/stdout
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);
}

int main (int argc, char* argv[]) {
    char *k1, *k2, *convertToLong, *convertedMsg = NULL, *message = NULL;
    int i, endRead = 0, endReadStdin = 0, readChars = 0;
    int *key_y = NULL, *key_x = NULL, size_y = 0, size_x = 0, m = DEFAULT_ALPHABET_SIZE;
    int modo = 0; // modo en 0 para cifrar y 1 para descifrar
    MatrixMN* matrixMsg = NULL, *matrixConvertedMsg = NULL;
    FILE *input = stdin, *output = stdout;

    // Comprobar número de argumentos del usuario
    if (argc < 4 || argc > 12) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();
        return -1;
    }

    // Obtener parámetros del usuario
    // permutacion {-C|-D} {-k1 K1 -k2 K2} [-m |Zm|] [-i filein] [-o fileout]
    for (i = 1; i < argc; i++) {
        if (strcmp("-C", argv[i]) == 0) {
            // Cifrar
            modo = 0;

        } else if (strcmp("-D", argv[i]) == 0) {
            // Descifrar
            modo = 1;

        } else if (strcmp(argv[i], "-k1") == 0) {
            // Vector de números para permutar las filas de P
            k1 = argv[++i];

            // Comprobamos si el siguiente argumento es k2
            if (strcmp(argv[++i], "-k2") == 0) {
                // Vector de números para permutar las columnas de P
                k2 = argv[++i];

            } else {
                printf("Error: El parámetro -k2 no existe.\n");

                close_files(input, output);
                return -1;
            }

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
    if ((m = load_alphabet(m)) == -1) {
        printf("Error: El alfabeto no se pudo cargar.\n");

        close_files(input, output);
        return -1;
    }

    // Comprobamos que la clave para permutar las filas de P es correcta
    key_y = get_key(k1, &size_y);
    if (key_y == NULL) {
        printf("Error: La clave k1 debe estar estar separada por comas y ");
        printf("contener todos los valores de 0 a N-1 sin repetir. Ej: 0,3,2,1\n");

        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    // Comprobamos que la clave para permutar las columnas de P es correcta
    key_x = get_key(k2, &size_x);
    if (key_x == NULL) {
        printf("Error: La clave k2 debe estar estar separada por comas y ");
        printf("contener todos los valores de 0 a N-1 sin repetir. Ej: 1,4,0,2,3\n");

        free_mem(key_y, key_x, message);
        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    // Inicializamos el buffer del mensaje a leer
    message = (char *) calloc ((size_x * size_y + 1), sizeof(char));
    if (message == NULL) {
        printf("Error: No se ha podido inicializar la memoria del mensaje a leer.\n");

        free_mem(key_y, key_x, message);
        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    // Generamos las claves para descifrar
    if (modo == 1) {
        if (decrypt_keys(key_y, key_x, size_y, size_x) == -1){
            printf("Error: Las claves de descifrado no se han generado correctamente.\n");

            free_mem(key_y, key_x, message);
            destroy_alphabet();
            close_files(input, output);
            return -1;
        }
    }

    // Leemos el mensaje a cifrar/descifrar
    while (!endRead && !endReadStdin) {
        if (fgets(message + readChars, ((size_x * size_y) - readChars + 1), input) == NULL) {
            // Error al leer el mensaje
            if (ferror(input)) {
                printf("Error: No se ha podido leer el mensaje del archivo.\n");

                free_mem(key_y, key_x, message);
                destroy_alphabet();
                close_files(input, output);
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

        // Añadimos padding al mensaje para que tenga el tamaño de la matriz M, N
        if (pad(message, size_x * size_y) == -1) {
            printf("Error: No se ha podido realizar el padding del mensaje correctamente.\n");

            free_mem(key_y, key_x, message);
            destroy_alphabet();
            close_files(input, output);
            return -1;
        }

        // Convertimos el mensaje leído a una matriz
        matrixMsg = load_text_to_matrix(message, size_y, size_x);
        if (matrixMsg == NULL) {
            printf("Error: No se ha podido convertir el mensaje a una matriz.\n");

            free_mem(key_y, key_x, message);
            destroy_alphabet();
            close_files(input, output);
            return -1;
        }
        
        // Ciframos/Desciframos el mensaje
        matrixConvertedMsg = permutate_encrypt(matrixMsg, key_y, key_x);

        convertedMsg = load_matrix_to_text(matrixConvertedMsg);
        if (convertedMsg == NULL) {
            printf("Error: No se ha podido convertir el mensaje %s\n", message);

            // Liberamos las matrices
            destroy_matrixMN(matrixMsg);
            if (matrixConvertedMsg != NULL) destroy_matrixMN(matrixConvertedMsg);

            free_mem(key_y, key_x, message);
            destroy_alphabet();
            close_files(input, output);
            return -1;
        }

        // Mostramos el mensaje convertido
        fprintf(output, "%s", convertedMsg);

        // Liberamos la memoria
        destroy_matrixMN(matrixMsg);
        destroy_matrixMN(matrixConvertedMsg);
        free(convertedMsg);

        // Limpiamos el buffer del mensaje
        memset(message, 0, size_x * size_y + 1);
    }

    if (output == stdout) fprintf(output, "\n");

    // Liberamos memoria
    free_mem(key_y, key_x, message);

    // Liberamos alfabeto
    destroy_alphabet();

    // Cerramos los archivos si no son stdin/stdout
    close_files(input, output);

    return 0;
}

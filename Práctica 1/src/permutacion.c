#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/alphabet.h"
#include "../includes/matrix.h"
#include "../includes/padding.h"

#define MAX_MESSAGE_SIZE 100

int yPermutate(Matrix* original, Matrix* permutated, int* key_y) {
    int i, j;

    if (!original || !permutated || !key_y) {
        return -1;
    }

    for (i = 0; i < get_matrix_size(original); i++) {
        for (j = 0; j < get_matrix_size(original); j++) {
            set_matrix_cell(permutated, j, key_y[i], get_matrix_cell(original, j, i));
        }
    }

    return 0;
}

int xPermutate(Matrix* original, Matrix* permutated, int* key_x) {
    int i, j;

    if (!original || !permutated || !key_x) {
        return -1;
    }

    for (i = 0; i < get_matrix_size(original); i++) {
        for (j = 0; j < get_matrix_size(original); j++) {
            set_matrix_cell(permutated, key_x[i], j, get_matrix_cell(original, i, j));
        }
    }

    return 0;
}

Matrix* permutate_encrypt(Matrix* matrix, int* key_y, int* key_x) {
    Matrix* halfPermutated = NULL;
    Matrix* permutated = NULL;

    if (!matrix || !key_x || !key_y) {
        return NULL;
    }

    halfPermutated = init_matrix(get_matrix_size(matrix));
    if (halfPermutated == NULL) return NULL;

    if (yPermutate(matrix, halfPermutated, key_y) == -1) {
        destroy_matrix(halfPermutated);
        return NULL;
    }

    permutated = init_matrix(get_matrix_size(matrix));
    if (permutated == NULL) return NULL;

    if (xPermutate(halfPermutated, permutated, key_x) == -1) {
        destroy_matrix(halfPermutated);
        destroy_matrix(permutated);
        return NULL;
    }

    destroy_matrix(halfPermutated);

    return permutated;
}

Matrix* permutate_decrypt(Matrix* matrix, int* key_y, int* key_x){
    Matrix *m = NULL;
    int i, *key_y2 = NULL, *key_x2 = NULL;

    // Creamos dos arrays auxiliares para guardar las claves temporalmente.
    key_y2 = (int*) malloc (get_matrix_size(matrix)*sizeof(int));
    key_x2 = (int*) malloc (get_matrix_size(matrix)*sizeof(int));

    // Calculamos la clave que invertiría la permutación cifrada.
    for(i = 0; i < get_matrix_size(matrix); i++){
        key_y2[key_y[i]] = i;
        key_x2[key_x[i]] = i;
    }

    // Cargamos los valores de los arrays auxiliares al array original.
    for(i = 0; i < get_matrix_size(matrix); i++){
        key_y[i] = key_y2[i];
        key_y[i] = key_y2[i];
    }

    // Liberamos los arrays auxiliares.
    free(key_y2);
    free(key_x2);

    // Ciframos con las claves inversas, lo cual da lugar al texto plano.
    m = permutate_encrypt(matrix, key_y, key_x);
    
    return m;
}

Matrix* load_text_to_matrix(char* block, int size) {
    Matrix* matrix = NULL;
    int i, j;

    if (!block || size < 1) {
        return NULL;
    }

    matrix = init_matrix(size);
    if (matrix == NULL) {
        return NULL;
    }

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (set_matrix_cell(matrix, j, i, get_letter_code(block[i*size+j])) == -1) {
                return NULL;
            }
        }
    }

    return matrix;
}

char* load_matrix_to_text(Matrix* matrix) {
    char* block;
    int i, j, size;

    if (!matrix) {
        return NULL;
    }

    size = get_matrix_size(matrix);
    if (size == -1) return NULL;

    block = (char*) calloc ((size+1), sizeof(char));
    if (block == NULL) {
        return NULL;
    }

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            block[i*size+j] = get_letter(get_matrix_cell(matrix, j, i));
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
    if (k == NULL) return NULL;

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
    char *k1, *k2, *convertToLong, *message = NULL;
    int i, *key_y = NULL, *key_x = NULL, size_y = 0, size_x = 0, m = DEFAULT_ALPHABET_SIZE;
    int modo = 0; // modo en 0 para cifrar y 1 para descifrar
    Matrix* matrix = NULL, *matrix2 = NULL;
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
    message = (char *) malloc ((MAX_MESSAGE_SIZE + 1) * sizeof(char));
    if (message == NULL) {
        printf("Error: No se ha podido inicializar la memoria del mensaje a leer.\n");

        free_mem(key_y, key_x, message);
        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    matrix = load_text_to_matrix("ABCDEFGHIJKLMNOP", size_y);

    matrix2 = permutate_encrypt(matrix, key_y, key_x);

    message = load_matrix_to_text(matrix2);

    printf("Message: %s\n", message);

    destroy_matrix(matrix);
    destroy_matrix(matrix2);

    // Liberamos memoria
    free_mem(key_y, key_x, message);

    // Liberamos alfabeto
    destroy_alphabet();

    // Cerramos los archivos si no son stdin/stdout
    close_files(input, output);

    return 0;
}

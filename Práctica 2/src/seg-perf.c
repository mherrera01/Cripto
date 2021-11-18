#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../includes/alphabet.h"
#include "../includes/euclides.h"

#define MAX_MESSAGE_SIZE 100

char affine_encrypt_char(char message, int a, int b, int m) {
    int messageCode;

    // Control de errores
    if (m <= 0) return '_';

    // Convertimos a y b al espacio Zm
    a %= m;
    if (a < 0) {
        a += m;
    }

    b %= m;
    if (b < 0) {
        b += m;
    }

    // Comprobamos si hay inverso multiplicativo
    if (get_modular_intverse(m, a) == 0) {
        printf("%d no tiene inverso multiplicativo.\n", a);
        return '_';
    }

    messageCode = get_letter_code(message);
    if (messageCode == -1) return '_'; // Caracter desconocido

    messageCode = ((messageCode*a) + b) % m;
    return get_letter(messageCode);
}

int get_affine_keys(int* a, int* b, int m, int modo) {
    // Control de errores
    if (a == NULL || b == NULL || modo > 1 || modo < 0 || m <= 0) {
        return -1;
    }

    *a = 0;
    
    // Generamos un número aleatorio a que tenga inverso en m y lo modificamos si quisiéramos que no fuera equiprobable
    while (get_modular_intverse(*a, m) < 1) {
        *a = rand() % m;
        if (modo == 1) {
            *a = *a / 3;
        }
    }

    // Generamos una variable b igual que la a, pero sin la necesidad de que tenga inverso en m, ya que no o necesita
    *b = rand() % m;
    if (modo == 1) {
        *b = *b % 3;
    }

    return 0;
}

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./seg-perf {-P|-I} [-m |Zm|] [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-P se utiliza el método equiprobable\n");
    printf("-I se utiliza el método no equiprobable\n");
    printf("-m tamaño del espacio de texto cifrado\n");
    printf("-i fichero de entrada\n");
    printf("-o fichero de salida\n");
    printf("---------------------------\n");
}

// Libera la memoria inicializada
void free_mem (int **probabilityTable, int tableSize, int *probabilityInPlaintext, int *probabilityInCiphertext, char *message) {
    int i;

    if (probabilityTable != NULL) {
        // Liberamos cada puntero de la tabla de probabilidades
        for (i = 0; i < tableSize; i++) {
            if (probabilityTable[i] != NULL) {
                free(probabilityTable[i]);
            }
        }

        free(probabilityTable);
    }

    if (probabilityInPlaintext != NULL) free(probabilityInPlaintext);
    if (probabilityInCiphertext != NULL) free(probabilityInCiphertext);
    if (message != NULL) free(message);
}

// Cerramos los archivos de entrada y salida abiertos previamente
void close_files (FILE *input, FILE *output) {
    if (input == NULL || output == NULL) return;

    // Cerramos los archivos si no son stdin/stdout
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);
}

int main (int argc, char *argv[]) {
    int i, j, a = 0, b = 0, m = DEFAULT_ALPHABET_SIZE;
    int messageLength = 0, readChars = 0, endRead = 0, endReadStdin = 0;
    int modo = 0; // modo en 0 para método equiprobable y 1 para no equiprobable
    FILE *input = stdin, *output = stdout;
    char encryptedCharacter, *convertToLong, *message = NULL;
    int **probabilityTable = NULL, *probabilityInPlaintext = NULL, *probabilityInCiphertext = NULL;

    // Comprobar número de argumentos del usuario
    if (argc < 2 || argc > 8) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();
        return -1;
    }

    // Obtener parámetros del usuario
    // seg-perf {-P|-I} [-m |Zm|] [-i filein] [-o fileout]
    for (i = 1; i < argc; i++) {
        if (strcmp("-P", argv[i]) == 0) {
            // Método equiprobable
            modo = 0;

        } else if (strcmp("-I", argv[i]) == 0) {
            // Método no equiprobable
            modo = 1;

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
    if ((m = load_alphabet("alphabet/alphabet.txt", m)) == -1) {
        printf("Error: El alfabeto no se pudo cargar.\n");

        close_files(input, output);
        return -1;
    }

    // Inicializamos memoria para la tabla de probabilidades
    probabilityTable = (int**) malloc (m * sizeof(int*));
    if (probabilityTable == NULL) {
        printf("Error: No se ha podido inicializar la memoria de la tabla de probabilidades.\n");

        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    // Inicializamos memoria para los array de la tabla de probabilidades
    for (i = 0; i < m; i++) {
        probabilityTable[i] = (int*) calloc (m, sizeof(int));
        if (probabilityTable[i] == NULL) {
            printf("Error: No se ha podido inicializar la memoria de los array en la tabla de probabilidades.\n");

            free_mem(probabilityTable, m, probabilityInPlaintext, probabilityInCiphertext, message);
            destroy_alphabet();
            close_files(input, output);
            return -1;
        }
    }

    // Inicializamos memoria para las probabilidades de cada letra en el alfabeto
    probabilityInPlaintext = (int*) calloc (m, sizeof(int));
    probabilityInCiphertext = (int*) calloc (m, sizeof(int));
    if (probabilityInPlaintext == NULL || probabilityInCiphertext == NULL) {
        printf("Error: No se ha podido inicializar la memoria para las probabilidades de cada letra en el alfabeto.\n");

        free_mem(probabilityTable, m, probabilityInPlaintext, probabilityInCiphertext, message);
        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    // Utilizamos srand para tener números aleatorios en cada ejecución del programa
    srand(time(NULL));

    // Inicializamos el buffer del mensaje a leer
    message = (char *) calloc ((MAX_MESSAGE_SIZE + 1), sizeof(char));
    if (message == NULL) {
        printf("Error: No se ha podido inicializar la memoria del mensaje a leer.\n");

        free_mem(probabilityTable, m, probabilityInPlaintext, probabilityInCiphertext, message);
        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    // Leemos el mensaje a cifrar
    while (!endRead && !endReadStdin) {
        if (fgets(message + readChars, (MAX_MESSAGE_SIZE - readChars + 1), input) == NULL) {
            // Error al leer el mensaje
            if (ferror(input)) {
                printf("Error: No se ha podido leer el mensaje del archivo.\n");

                free_mem(probabilityTable, m, probabilityInPlaintext, probabilityInCiphertext, message);
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

        // Ciframos caracter a caracter con el cifrado afin
        for (i = 0; i < strlen(message); i++) {
            if (get_affine_keys(&a, &b, m, modo) == -1) {
                printf("Error: No se han podido crear las claves.\n");

                free_mem(probabilityTable, m, probabilityInPlaintext, probabilityInCiphertext, message);
                destroy_alphabet();
                close_files(input, output);
                return -1;
            }
            encryptedCharacter = affine_encrypt_char(message[i], a, b, m);

            // Registramos el dato en la tabla para hallar la probabilidad después
            if (get_letter_code(encryptedCharacter) != -1 && get_letter_code(message[i]) != -1) {
                probabilityTable[get_letter_code(message[i])][get_letter_code(encryptedCharacter)]++;
                messageLength++;
            }
        }

        // Limpiamos el buffer del mensaje
        memset(message, 0, MAX_MESSAGE_SIZE + 1);
    }

    if (output == stdout) fprintf(output, "\nCalculando las probabilidades de las letras...\n");

    // Calculamos las probabilidades de cada letra en el texto plano y cifrado
    for (i = 0; i < m; i++){
        for (j = 0; j < m; j++){
            probabilityInPlaintext[i] += probabilityTable[i][j];
            probabilityInCiphertext[i] += probabilityTable[j][i];
        }
    }

    if (output == stdout) fprintf(output, "OK: Probabilidades de las letras calculadas.\n\n");

    /* Calculamos e imprimimos las probabilidades de las letras en texto plano para compararlas con
    las condicionadas por las letras cifradas */
    for (i = 0; i < m; i++) {
        fprintf(output, "P(%c) = %lf\n", get_letter(i), ((double)probabilityInPlaintext[i])/messageLength);
        for (j = 0; j < m; j++) {
            fprintf(output, "P(%c|%c) = %lf\t", get_letter(i), get_letter(j), ((double)probabilityTable[i][j])/probabilityInCiphertext[i]);
        }
        fprintf(output, "\n");
    }

    // Liberamos memoria
    free_mem(probabilityTable, m, probabilityInPlaintext, probabilityInCiphertext, message);

    // Liberamos alfabeto
    destroy_alphabet();

    // Cerramos los archivos si no son stdin/stdout
    close_files(input, output);

    return 0;
}

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
            *a = *a % 3;
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

// Cerramos los archivos de entrada y salida abiertos previamente
void close_files (FILE *input, FILE *output) {
    if (input == NULL || output == NULL) return;

    // Cerramos los archivos si no son stdin/stdout
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);
}

int main (int argc, char *argv[]) {
    int i, endRead = 0, readChars = 0, a = 0, b = 0, m = DEFAULT_ALPHABET_SIZE;
    int modo = 0; // modo en 0 para método equiprobable y 1 para no equiprobable
    FILE *input = stdin, *output = stdout;
    char *convertToLong, *message = NULL;

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

    // Utilizamos srand para tener números aleatorios en cada ejecución del programa
    srand(time(NULL));

    // Inicializamos el buffer del mensaje a leer
    message = (char *) calloc ((MAX_MESSAGE_SIZE + 1), sizeof(char));
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

        // Ciframos caracter a caracter con el cifrado afin
        for (i = 0; i < readChars; i++) {
            if (get_affine_keys(&a, &b, m, modo) == -1) {
                printf("Error: No se han podido crear las claves.\n");

                free(message);
                destroy_alphabet();
                close_files(input, output);
                return -1;
            }
            fprintf(output, "%c", affine_encrypt_char(message[i], a, b, m));
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

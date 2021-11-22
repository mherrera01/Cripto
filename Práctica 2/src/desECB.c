#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../includes/bits.h"
#include "../includes/des.h"

#define BLOCK_SIZE 8 // Número de bytes en los que se divide el texto a cifrar/descifrar

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("desECB {-C | -D -k clave} [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-C el programa cifra\n");
    printf("-D el programa descifra\n");
    printf("-k clave de 64 bits: 56 bits + 8 bits de paridad en hexadecimal. Ej: 3119DC7A6237ECBC)\n");
    printf("-i fichero de entrada\n");
    printf("-o fichero de salida\n");
    printf("---------------------------\n");
}

// Libera la memoria inicializada
void free_mem (Bits *key, Bits *block, char *message) {
    if (key != NULL) destroy_bits(key);
    if (block != NULL) destroy_bits(block);
    if (message != NULL) free(message);
}

// Cerramos los archivos de entrada y salida abiertos previamente
void close_files (FILE *input, FILE *output) {
    if (input == NULL || output == NULL) return;

    // Cerramos los archivos si no son stdin/stdout
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);
}

int main(int argc, char *argv[]) {
    Bits *key = NULL, *block = NULL, *convertedBlock = NULL;
    char *k, *message = NULL, keyHex[2] = {0}, keyHexValue;
    int i, endRead = 0, readChars = 0;
    int modo = 0; // modo en 0 para cifrar y 1 para descifrar
    FILE *input = stdin, *output = stdout;

    // Comprobar número de argumentos del usuario
    if (argc < 2 || argc > 8) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();
        return -1;
    }

    // Obtener parámetros del usuario
    // desECB {-C | -D -k clave} [-i filein] [-o fileout]
    for (i = 1; i < argc; i++) {
        if (strcmp("-C", argv[i]) == 0) {
            // Cifrar
            modo = 0;

        } else if (strcmp("-D", argv[i]) == 0) {
            // Descifrar
            modo = 1;

            // Comprobamos si el siguiente argumento es k
            if (strcmp(argv[++i], "-k") == 0) {
                // Clave de 64 bits: 56 bits + 8 bits de paridad
                k = argv[++i];
                if (strlen(k) != 64/4) {
                    printf("Error: La clave -k debe estar en hexadecimal y contener 64 bits. Ej: 3119DC7A6237ECBC.\n");

                    close_files(input, output);
                    return -1;
                }

            } else {
                printf("Error: El parámetro -k no existe.\n");

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

    // Utilizamos srand para tener números aleatorios en cada ejecución del programa
    srand(time(NULL));

    // Creamos una clave aleatoria para cifrar
    if (modo == 0) {
        // Clave de 64 bits (56 + 8)
        key = generate_random_desKey();
        if (key == NULL) {
            printf("Error: La clave aleatoria necesaria para cifrar no se pudo generar.\n");

            close_files(input, output);
            return -1;
        }

        printf("Clave generada:\n");
        print_bits_hex(key);
        printf("\n");
    } else {
        // Reservamos memoria para la clave de descifrado
        key = init_bits(64);
        if (key == NULL) {
            printf("Error: No se pudo reservar memoria para la clave de descifrado.\n");

            close_files(input, output);
            return -1;
        }

        // Obtenemos la clave para descifrar pasada como argumento
        for (i = 0; i < strlen(k); i++) {
            // Convertimos la clave en ASCII a valores hexadecimales
            strncpy(keyHex, &k[i], 1);
            keyHexValue = (char) strtol(keyHex, NULL, 16);

            if (set_hex_to_value(key, i, keyHexValue) == -1) {
                printf("Error: La clave necesaria para descifrar no se pudo generar.\n");

                free_mem(key, block, message);
                close_files(input, output);
                return -1;
            }
        }
    }

    // Creamos el bloque de bytes para el mensaje
    block = init_bits(64);
    if (block == NULL) {
        printf("Error: No se ha podido inicializar la memoria del bloque de bytes para el mensaje.\n");

        free_mem(key, block, message);
        close_files(input, output);
        return -1;
    }

    // Inicializamos el buffer del mensaje a leer
    message = (char *) calloc ((BLOCK_SIZE + 1), sizeof(char));
    if (message == NULL) {
        printf("Error: No se ha podido inicializar la memoria del mensaje a leer.\n");

        free_mem(key, block, message);
        close_files(input, output);
        return -1;
    }

    // Mensajes de estado del programa
    if (input == stdin) {
        printf("Introduce el texto a cifrar/descifrar: \n");
    } else {
        printf("Ejecutando DES en modo de operación ECB...\n");
    }

    // Leemos el mensaje a cifrar/descifrar
    while (1) {
        // Leemos en stdin con fgets para terminar en un salto de línea
        if (input == stdin) {
            if (fgets(message, (BLOCK_SIZE + 1), input) == NULL) break;

            // En el salto de línea dejamos de leer
            endRead = (strchr(message, '\n') != NULL);
            if (endRead) message[strcspn(message, "\n")] = '\0';

            readChars = strlen(message);

        // Leemos archivos binarios con fread
        } else {
            readChars = fread(message, sizeof(char), BLOCK_SIZE, input);
            if (readChars <= 0) break;
        }

        // Asignamos los caracteres del bloque al bloque de bytes
        for (i = 0; i < readChars; i++) {
            if (set_byte_to_value(block, i, message[i]) == -1){
                printf("Error: No se ha podido asignar el valor del caracter %c al bloque.\n", message[i]);

                free_mem(key, block, message);
                close_files(input, output);
                return -1;
            }
        }

        if (modo == 0) {
            // Ciframos el bloque con des
            convertedBlock = des_encrypt(block, key);
        } else {
            // Desciframos el bloque con des
            convertedBlock = des_decrypt(block, key);
        }

        // Comprobamos si el bloque se ha podido convertir
        if (convertedBlock == NULL) {
            printf("Error: No se ha podido convertir el bloque:\n");
            print_bits_hex(block);

            free_mem(key, block, message);
            close_files(input, output);
            return -1;
        }

        // Mostramos el mensaje convertido de 64 bits (8 bytes)
        for (i = 0; i < 8; i++) {
            fprintf(output, "%c", get_byte_value(convertedBlock, i));
        }

        // Liberamos la memoria del mensaje convertido
        destroy_bits(convertedBlock);

        // Limpiamos el buffer del mensaje y el bloque de bytes
        memset(message, 0, BLOCK_SIZE + 1);
        if (set_bits_to_zero(block) == -1) {
            printf("Error: No se ha podido limpiar el bloque de bytes.\n");

            free_mem(key, block, message);
            close_files(input, output);
            return -1;
        }

        // Salir del bucle en stdin
        if (endRead) break;
    }

    if (input == stdin || output == stdout) printf("\n");
    printf("DES en modo de operación ECB ejecutado con éxito.\n");

    // Liberamos memoria
    free_mem(key, block, message);

    // Cerramos los archivos si no son stdin/stdout
    close_files(input, output);

    return 0;
}

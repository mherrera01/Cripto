#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../includes/bits.h"
#include "../includes/des.h"

/**
 * Desplazamiento hacia la izquierda de s bits de una lista de bits.
 * 
 * Devuelve el resultado del desplazamiento en una lista de 64 bits o NULL en caso de error.
 */ 
Bits *shift_register(Bits *shift, int sBits) {
    Bits *result = NULL;
    int i, difference, newPos;

    // Control de errores
    if (shift == NULL || sBits <= 0 || sBits > 65) return NULL;

    // Reservamos memoria para el resultado del desplazamiento
    result = init_bits(64);
    if (result == NULL) return NULL;

    // Desplazamos a la izquierda el valor binario
    difference = 64 - get_bits_size(shift);
    for (i = 0; i < get_bits_size(shift); i++) {
        newPos = i - sBits + difference;
        if (newPos >= 0) {
            set_bit(result, newPos, get_bit(shift, i));
        }
    }

    return result;
}

Bits *compute_desCFB(Bits *registerEntry, int shifts, Bits *message, Bits *key) {
    Bits *convertedMsg = NULL;
    Bits *shiftBits = NULL, *desResult = NULL, *select = NULL, *discard = NULL;

    // Control de errores
    if (registerEntry == NULL || message == NULL || key == NULL) return NULL;
    if (get_bits_size(message) != shifts) return NULL;

    // Desplazamiento a la izquierda para obtener la entrada de 64 bits del des
    shiftBits = shift_register(registerEntry, shifts);
    if (shiftBits == NULL) {
        printf("Error: No se ha podido desplazar a la izquierda el bloque:\n");
        print_bits_hex(registerEntry);

        return NULL;
    }

    // Ciframos el resultado del desplazamiento con des
    desResult = des_encrypt(shiftBits, key);
    if (desResult == NULL) {
        printf("Error: No se ha podido cifrar el resultado del desplazamiento:\n");
        print_bits_hex(shiftBits);

        destroy_bits(shiftBits);
        return NULL;
    }

    // Seleccionamos los s bits más significativos del resultado de DES
    if (shifts != 64) {
        if (split_bits(desResult, shifts, &select, &discard) == -1) {
            printf("Error: No se han podido seleccionar los %d bits del resultado de DES:\n", shifts);
            print_bits_hex(desResult);

            destroy_bits(desResult);
            destroy_bits(shiftBits);
            return NULL;
        }

        // XOR del mensaje y la selección de los s bits
        convertedMsg = xor_bits(select, message);

        destroy_bits(select);
        destroy_bits(discard);
    } else {
        // Cogemos todos los bits del resultado de DES y hacemos XOR con el mensaje
        convertedMsg = xor_bits(desResult, message);
    }

    // Liberamos memoria
    destroy_bits(desResult);
    destroy_bits(shiftBits);

    return convertedMsg;
}

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("desCFB {-C | -D -k clave} {-iv IV} {-S s} [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-C el programa cifra\n");
    printf("-D el programa descifra\n");
    printf("-k clave de 64 bits: 56 bits + 8 bits de paridad en hexadecimal. Ej: 3119DC7A6237ECBC\n");
    printf("-iv vector de inicialización en hexadecimal. Ej: 76BA344C7F3B5E49\n");
    printf("-S número de bits");
    printf("-i fichero de entrada\n");
    printf("-o fichero de salida\n");
    printf("---------------------------\n");
}

// Libera la memoria inicializada
void free_mem (Bits *key, Bits *iv, Bits *registerEntry, Bits *block, char *message) {
    if (key != NULL) destroy_bits(key);
    if (iv != NULL) destroy_bits(iv);
    if (registerEntry != NULL) destroy_bits(registerEntry);
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
    Bits *key = NULL, *ivBits = NULL, *registerEntry = NULL, *block = NULL, *convertedBlock = NULL;
    char *k = NULL, *iv = NULL, *convertToLong, *message = NULL, hexChar[2] = {0}, hexValue;
    int i, endRead = 0, readChars = 0, s = 0, blockSize = 0, firstIteration = 0;
    int modo = 0; // modo en 0 para cifrar y 1 para descifrar
    FILE *input = stdin, *output = stdout;

    // Comprobar número de argumentos del usuario
    if (argc < 6 || argc > 12) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();
        return -1;
    }

    // Obtener parámetros del usuario
    // desCFB {-C | -D -k clave} {-iv IV} {-S s} [-i filein] [-o fileout]
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

        } else if (strcmp(argv[i], "-iv") == 0) {
            // Vector de inicialización
            iv = argv[++i];
            if (strlen(iv) != 64/4) {
                printf("Error: La clave -iv debe estar en hexadecimal y contener 64 bits. Ej: 76BA344C7F3B5E49.\n");

                close_files(input, output);
                return -1;
            }

        } else if (strcmp(argv[i], "-S") == 0) {
            // Número de bits
            s = (int) strtol(argv[++i], &convertToLong, 10);

            // Comprobamos si no se ha convertido ningún caracter o el valor no es válido
            if (argv[i] == convertToLong || s <= 0 || s > 64 || s % BYTE_SIZE != 0) {
                printf("Error: El parámetro -S %s no es válido. ", argv[i]);
                printf("El número de bits debe ser mayor que 0, menor que 65 y múltiplo de 8.\n");

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
        if (key == NULL || k == NULL) {
            printf("Error: No se pudo reservar memoria para la clave de descifrado.\n");

            close_files(input, output);
            return -1;
        }

        // Obtenemos la clave para descifrar pasada como argumento
        for (i = 0; i < strlen(k); i++) {
            // Convertimos la clave en ASCII a valores hexadecimales
            strncpy(hexChar, &k[i], 1);
            hexValue = (char) strtol(hexChar, NULL, 16);

            if (set_hex_to_value(key, i, hexValue) == -1) {
                printf("Error: La clave necesaria para descifrar no se pudo generar.\n");

                free_mem(key, ivBits, registerEntry, block, message);
                close_files(input, output);
                return -1;

            }
        }
    }

    // Reservamos memoria para el vector de incialización de 64 bits
    ivBits = init_bits(64);
    if (ivBits == NULL || iv == NULL) {
        printf("Error: No se pudo reservar memoria para el vector de inicialización.\n");

        free_mem(key, ivBits, registerEntry, block, message);
        close_files(input, output);
        return -1;
    }

    // Obtenemos el vector de inicialización pasado como argumento
    for (i = 0; i < strlen(iv); i++) {
        // Convertimos el vector de inicialización en ASCII a valores hexadecimales
        strncpy(hexChar, &iv[i], 1);
        hexValue = (char) strtol(hexChar, NULL, 16);

        if (set_hex_to_value(ivBits, i, hexValue) == -1) {
            printf("Error: El vector de inicialización no se pudo generar.\n");

            free_mem(key, ivBits, registerEntry, block, message);
            close_files(input, output);
            return -1;
        }
    }

    // Asignamos el número de bytes en los que se divide el texto a cifrar/descifrar
    blockSize = s/BYTE_SIZE;

    // Reservamos memoria para la entrada al registro
    registerEntry = init_bits(s);
    if (registerEntry == NULL) {
        printf("Error: No se ha podido inicializar la memoria para la entrada al registro.\n");

        free_mem(key, ivBits, registerEntry, block, message);
        close_files(input, output);
        return -1;
    }

    // Creamos el bloque de bytes para el mensaje
    block = init_bits(s);
    if (block == NULL) {
        printf("Error: No se ha podido inicializar la memoria del bloque de bytes para el mensaje.\n");

        free_mem(key, ivBits, registerEntry, block, message);
        close_files(input, output);
        return -1;
    }

    // Inicializamos el buffer del mensaje a leer
    message = (char *) calloc ((blockSize + 1), sizeof(char));
    if (message == NULL) {
        printf("Error: No se ha podido inicializar la memoria del mensaje a leer.\n");

        free_mem(key, ivBits, registerEntry, block, message);
        close_files(input, output);
        return -1;
    }

    // Mensajes de estado del programa
    if (input == stdin) {
        printf("Introduce el texto a cifrar/descifrar: \n");
    } else {
        printf("Ejecutando DES en modo de operación CFB...\n");
    }

    // Leemos el mensaje a cifrar/descifrar
    while (1) {
        // Leemos en stdin con fgets para terminar en un salto de línea
        if (input == stdin) {
            if (fgets(message, (blockSize + 1), input) == NULL) break;

            // En el salto de línea dejamos de leer
            endRead = (strchr(message, '\n') != NULL);
            if (endRead) message[strcspn(message, "\n")] = '\0';

            readChars = strlen(message);

        // Leemos archivos binarios con fread
        } else {
            readChars = fread(message, sizeof(char), blockSize, input);
            if (readChars <= 0) break;
        }

        // Asignamos los caracteres del bloque al bloque de bytes
        for (i = 0; i < readChars; i++) {
            if (set_byte_to_value(block, i, message[i]) == -1){
                printf("Error: No se ha podido asignar el valor del caracter %c al bloque.\n", message[i]);

                free_mem(key, ivBits, registerEntry, block, message);
                close_files(input, output);
                return -1;
            }
        }

        // Convertimos el bloque con des en modo de operación CFB
        if (firstIteration == 0) {
            // Usamos el IV en la primera iteración
            convertedBlock = compute_desCFB(ivBits, s, block, key);
            firstIteration = 1;
        } else {
            convertedBlock = compute_desCFB(registerEntry, s, block, key);
        }

        // Comprobamos si el bloque se ha podido convertir
        if (convertedBlock == NULL) {
            printf("Error: No se ha podido convertir el bloque:\n");
            print_bits_hex(block);

            free_mem(key, ivBits, registerEntry, block, message);
            close_files(input, output);
            return -1;
        }

        // Mostramos el mensaje convertido de s bits
        for (i = 0; i < blockSize; i++) {
            fprintf(output, "%c", get_byte_value(convertedBlock, i));
        }

        if (modo == 0) {
            // Asignamos el resultado a la siguiente entrada del registro
            set_bits_decimal_value(registerEntry, get_bits_decimal_value(convertedBlock));
        } else {
            // Asignamos el bloque del mensaje a la siguiente entrada del registro
            set_bits_decimal_value(registerEntry, get_bits_decimal_value(block));
        }

        // Liberamos la memoria del mensaje convertido
        destroy_bits(convertedBlock);

        // Limpiamos el buffer del mensaje y el bloque de bytes
        memset(message, 0, blockSize + 1);
        if (set_bits_to_zero(block) == -1) {
            printf("Error: No se ha podido limpiar el bloque de bytes.\n");

            free_mem(key, ivBits, registerEntry, block, message);
            close_files(input, output);
            return -1;
        }

        // Salir del bucle en stdin
        if (endRead) break;
    }

    if (input == stdin) printf("\n");
    printf("DES en modo de operación CFB ejecutado con éxito.\n");

    // Liberamos memoria
    free_mem(key, ivBits, registerEntry, block, message);

    // Cerramos los archivos si no son stdin/stdout
    close_files(input, output);

    return 0;
}

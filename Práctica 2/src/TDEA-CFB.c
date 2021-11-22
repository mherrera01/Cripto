#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../includes/bits.h"
#include "../includes/des.h"

// Devuelve una clave aleatoria de 192 bits (168 bits + 24 bits paridad) o NULL en caso de error.
Bits *generate_random_triple_desKey() {
    Bits *key = NULL, *key1 = NULL, *key2 = NULL, *key3 = NULL, *auxKey = NULL;

    // Creamos tres claves aleatorias de 64 bits cada una con sus correspondientes bits de paridad
    key1 = generate_random_desKey();
    key2 = generate_random_desKey();
    key3 = generate_random_desKey();

    // Control de errores
    if (key1 == NULL || key2 == NULL || key3 == NULL) {
        destroy_bits(key1);
        destroy_bits(key2);
        destroy_bits(key3);

        return NULL;
    }

    // Unimos las dos primeras claves aleatorias en 128 bits
    auxKey = merge_bits(key1, key2);
    if (auxKey == NULL) {
        destroy_bits(key1);
        destroy_bits(key2);
        destroy_bits(key3);

        return NULL;
    }

    // Obtenemos la clave final de 192 bits (24 de paridad)
    key = merge_bits(auxKey, key3);

    // Liberamos memoria
    destroy_bits(auxKey);
    destroy_bits(key1);
    destroy_bits(key2);
    destroy_bits(key3);

    return key;
}

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

Bits *compute_triple_desCFB(Bits *registerEntry, int shifts, Bits *message, Bits *key) {
    Bits *convertedMsg = NULL;
    Bits *shiftBits = NULL, *select = NULL, *discard = NULL;
    Bits *desResult1 = NULL, *desResult2 = NULL, *desResult3 = NULL;
    Bits *key1 = NULL, *key2 = NULL, *key3 = NULL, *auxKey = NULL; // Claves para el triple DES

    // Control de errores
    if (registerEntry == NULL || message == NULL || key == NULL) return NULL;
    if (get_bits_size(message) != shifts || get_bits_size(key) != 192) return NULL;

    // Desplazamiento a la izquierda para obtener la entrada de 64 bits del des
    shiftBits = shift_register(registerEntry, shifts);
    if (shiftBits == NULL) {
        printf("Error: No se ha podido desplazar a la izquierda el bloque:\n");
        print_bits_hex(registerEntry);

        return NULL;
    }

    // Obtenemos las primera clave a partir de 192 bits
    if (split_bits(key, 64, &key1, &auxKey) == -1) {
        printf("Error: No se ha podido obtener la primera clave necesaria para el triple DES.\n");

        destroy_bits(shiftBits);
        return NULL;
    }

    // Obtenemos la segunda y tercera clave para el triple DES
    if (split_bits(auxKey, 64, &key2, &key3) == -1) {
        printf("Error: No se ha podido obtener la segunda y tercera clave necesarias para el triple DES.\n");

        destroy_bits(key1);
        destroy_bits(auxKey);
        destroy_bits(shiftBits);
        return NULL;
    }
    destroy_bits(auxKey);

    // Ciframos el resultado del desplazamiento con DES y con la primera clave
    desResult1 = des_encrypt(shiftBits, key1);
    if (desResult1 == NULL) {
        printf("Error: No se ha podido cifrar el resultado del desplazamiento con la clave 1:\n");
        print_bits_hex(key1);

        destroy_bits(key1);
        destroy_bits(key2);
        destroy_bits(key3);
        destroy_bits(shiftBits);
        return NULL;
    }

    // Desciframos el resultado del primer cifrado DES con la segunda clave
    desResult2 = des_decrypt(desResult1, key2);
    if (desResult2 == NULL) {
        printf("Error: No se ha podido descifrar el resultado del primer cifrado DES con la clave 2:\n");
        print_bits_hex(key2);

        destroy_bits(desResult1);
        destroy_bits(key1);
        destroy_bits(key2);
        destroy_bits(key3);
        destroy_bits(shiftBits);
        return NULL;
    }

    // Ciframos el resultado del descifrado DES con la tercera clave
    desResult3 = des_encrypt(desResult2, key3);
    if (desResult3 == NULL) {
        printf("Error: No se ha podido cifrar el resultado del descifrado DES con la clave 3:\n");
        print_bits_hex(key3);

        destroy_bits(desResult1);
        destroy_bits(desResult2);
        destroy_bits(key1);
        destroy_bits(key2);
        destroy_bits(key3);
        destroy_bits(shiftBits);
        return NULL;
    }

    // Liberamos memoria
    destroy_bits(desResult1);
    destroy_bits(desResult2);
    destroy_bits(key1);
    destroy_bits(key2);
    destroy_bits(key3);

    // Seleccionamos los s bits más significativos del resultado del triple DES
    if (shifts != 64) {
        if (split_bits(desResult3, shifts, &select, &discard) == -1) {
            printf("Error: No se han podido seleccionar los %d bits del resultado del triple DES:\n", shifts);
            print_bits_hex(desResult3);

            destroy_bits(desResult3);
            destroy_bits(shiftBits);
            return NULL;
        }

        // XOR del mensaje y la selección de los s bits
        convertedMsg = xor_bits(select, message);

        destroy_bits(select);
        destroy_bits(discard);
    } else {
        // Cogemos todos los bits del resultado del triple DES y hacemos XOR con el mensaje
        convertedMsg = xor_bits(desResult3, message);
    }

    // Liberamos memoria
    destroy_bits(desResult3);
    destroy_bits(shiftBits);

    return convertedMsg;
}

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("TDEA-CFB {-C | -D -k clave} {-iv vectorinicializacion} {-S s} [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-C el programa cifra\n");
    printf("-D el programa descifra\n");
    printf("-k clave de 192 bits: 168 bits + 24 bits de paridad en hexadecimal. Ej: E6B9C11C7AFE94BAA7DC0E5E5E3270DC0D16A4589B8F1C26\n");
    printf("-iv vector de inicialización en hexadecimal. Ej: 76BA344C7F3B5E49\n");
    printf("-S número de bits para el desplazamiento en modo CFB\n");
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
    // TDEA-CFB {-C | -D -k clave} {-iv vectorinicializacion} {-S s} [-i filein] [-o fileout]
    for (i = 1; i < argc; i++) {
        if (strcmp("-C", argv[i]) == 0) {
            // Cifrar
            modo = 0;

        } else if (strcmp("-D", argv[i]) == 0) {
            // Descifrar
            modo = 1;

            // Comprobamos si el siguiente argumento es k
            if (strcmp(argv[++i], "-k") == 0) {
                // Clave de 192 bits: 168 bits + 24 bits de paridad
                k = argv[++i];
                if (strlen(k) != 192/4) {
                    printf("Error: La clave -k debe estar en hexadecimal y contener 192 bits. Ej: E6B9C11C7AFE94BAA7DC0E5E5E3270DC0D16A4589B8F1C26.\n");

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
            // Número de bits para el desplazamiento en modo CFB
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

    // Utilizamos srand para tener números aleatorios en cada ejecución del programa
    srand(time(NULL));

    // Creamos una clave aleatoria para cifrar
    if (modo == 0) {
        // Clave de 192 bits (168 + 24)
        key = generate_random_triple_desKey();
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
        key = init_bits(192);
        if (key == NULL || k == NULL) {
            printf("Error: No se pudo reservar memoria para la clave de descifrado.\n");

            free_mem(key, ivBits, registerEntry, block, message);
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
        printf("Ejecutando triple DES en modo de operación CFB...\n");
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

        // Convertimos el bloque con triple des en modo de operación CFB
        if (firstIteration == 0) {
            // Usamos el IV en la primera iteración
            convertedBlock = compute_triple_desCFB(ivBits, s, block, key);
            firstIteration = 1;
        } else {
            convertedBlock = compute_triple_desCFB(registerEntry, s, block, key);
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

    if (input == stdin || output == stdout) printf("\n");
    printf("Triple DES en modo de operación CFB ejecutado con éxito.\n");

    // Liberamos memoria
    free_mem(key, ivBits, registerEntry, block, message);

    // Cerramos los archivos si no son stdin/stdout
    close_files(input, output);

    return 0;
}

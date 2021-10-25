#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/alphabet.h"

char* vigenere_encrypt(char *message, char *key, int m) {
    int i, encryptedLetter, originalLetter, keyLetter;
    char* encryptedMessage = NULL;

    // Control de errores
    if (message == NULL || key == NULL || m <= 0 || strlen(message) > strlen(key)) {
        return NULL;
    }

    // Reservamos espacio para el mensaje cifrado
    encryptedMessage = (char*) calloc (strlen(message)+1, sizeof(char));
    if (encryptedMessage == NULL) return NULL;

    // Iteramos por los caracteres dentro del bloque
    for (i = 0; i < strlen(message); i++) {
        originalLetter = get_letter_code(message[i]);
        keyLetter = get_letter_code(key[i]);

        // En caso de que sean un caracter inválido, ponemos _.
        if (originalLetter == -1 || keyLetter == -1) {
            encryptedMessage[i] = '_';
            continue;
        }

        // Ciframos el caracter
        encryptedLetter = originalLetter + keyLetter;
        encryptedMessage[i] = get_letter(encryptedLetter % m);
    }

    return encryptedMessage;
}

char* decrypt_key(char *key, int m) {
    int i;

    // Control de errores.
    if (key == NULL || m <= 0) return NULL;

    // Cambiamos el valor de la clave para que deshaga los cambios de la encriptación con su inverso aditivo.
    for (i = 0; i < strlen(key); i++) {
        key[i] = get_letter((m - get_letter_code(key[i])) % m);
    }

    return key;
}

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./vigenere {-C|-D} {-k clave} [-m |Zm|] [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-C el programa cifra\n");
    printf("-D el programa descifra\n");
    printf("-k cadena de caracteres usada como clave\n");
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
    char *k, *convertToLong, *convertedMsg = NULL, *message = NULL;
    int modo = 0; // modo en 0 para cifrar y 1 para descifrar
    int i, endRead = 0, readChars = 0, keyLength = 0, m = DEFAULT_ALPHABET_SIZE;
    FILE *input = stdin, *output = stdout;

    // Comprobar número de argumentos del usuario
    if (argc < 4 || argc > 10) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();
        return -1;
    }

    // Obtener parámetros del usuario
    // vigenere {-C|-D} {-k clave} [-m |Zm|] [-i filein] [-o fileout]
    for (i = 1; i < argc; i++) {
        if (strcmp("-C", argv[i]) == 0) {
            // Cifrar
            modo = 0;

        } else if (strcmp("-D", argv[i]) == 0) {
            // Descifrar
            modo = 1;

        } else if (strcmp(argv[i], "-k") == 0) {
            // Cadena de caracteres usada como clave
            k = argv[++i];
        
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

    // Obtenemos la longitud de la clave para cifrar en grupos
    keyLength = strlen(k);
    if (keyLength <= 0) {
        printf("Error: La clave no es válida.\n");

        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    // Inicializamos el buffer del mensaje a leer con la longitud de la clave
    message = (char *) calloc ((keyLength + 1), sizeof(char));
    if (message == NULL) {
        printf("Error: No se ha podido inicializar la memoria del mensaje a leer.\n");

        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    // Invertimos la clave si vamos a descifrar
    if (modo == 1) {
        if (decrypt_key(k, m) == NULL) {
            printf("Error: No se ha podido generar la clave para descifrar.\n");

            free(message);
            destroy_alphabet();
            close_files(input, output);
            return -1;
        }
    }

    // Leemos el mensaje a cifrar/descifrar
    while (fgets(message + readChars, (keyLength - readChars + 1), input) != NULL) {
        // Si la entrada es stdin en el salto de línea dejamos de leer
        endRead = (input == stdin) && (strchr(message, '\n') != NULL);
        if (endRead) message[strcspn(message, "\n")] = '\0';

        /* Como fgets deja de leer cuando encuentra un salto de línea, continuamos
        leyendo hasta rellenar el bloque del mensaje si la entrada no es stdin */
        if (!(input == stdin) && strchr(message, '\n') != NULL) {
            readChars = strcspn(message, "\n") + 1; // Asignamos por donde debemos seguir leyendo
            message[readChars - 1] = ' '; // Sustituimos el salto de línea por un espacio
            continue;
        } else readChars = 0;

        // Ciframos/Desciframos el mensaje
        convertedMsg = vigenere_encrypt(message, k, m);
        if (convertedMsg == NULL) {
            printf("Error: No se ha podido convertir el mensaje %s\n", message);

            free(message);
            destroy_alphabet();
            close_files(input, output);
            return -1;
        }

        // Mostramos el mensaje convertido
        fprintf(output, "%s", convertedMsg);

        // Liberamos la memoria del mensaje convertido
        free(convertedMsg);

        // Limpiamos el buffer del mensaje
        memset(message, 0, keyLength + 1);

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

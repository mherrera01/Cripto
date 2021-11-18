#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../includes/alphabet.h"
#include "../includes/byte.h"

/*
char** divideBlock(char* key){
    int i;
    char *keya, *keyb, **keys;

    keya = (char*) malloc (32*sizeof(char));
    keyb = (char*) malloc (32*sizeof(char));
    keys = (char**) malloc (2*sizeof(char*));

    for(i = 0; i < 64; i++){
        if(i<32){
            keya[i] = key[i];
        }
        else{
            keyb[i-32] = key[i];
        }
    }

    keys[0] = keya;
    keys[1] = keyb;

    return keys;

}
*/

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("desCFB {-C | -D -k clave} {-S s} [-m |Zm|] [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-C el programa cifra\n");
    printf("-D el programa descifra\n");
    printf("-k clave de 64 bits: 56 bits + 8 bits de paridad\n");
    printf("-S número de bits");
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

int main(int argc, char *argv[]) {
    char *k, *convertToLong, *byte = NULL;
    int i, s = 0, m = DEFAULT_ALPHABET_SIZE;
    int modo = 0; // modo en 0 para cifrar y 1 para descifrar
    FILE *input = stdin, *output = stdout;

    // Comprobar número de argumentos del usuario
    if (argc < 4 || argc > 12) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();
        return -1;
    }

    // Obtener parámetros del usuario
    // desCFB {-C | -D -k clave} {-S s} [-m |Zm|] [-i filein] [-o fileout]
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

            } else {
                printf("Error: El parámetro -k no existe.\n");

                close_files(input, output);
                return -1;
            }

        } else if (strcmp(argv[i], "-S") == 0) {
            // Número de bits
            s = (int) strtol(argv[++i], &convertToLong, 10);

            // Comprobamos si no se ha convertido ningún caracter
            if (argv[i] == convertToLong) {
                printf("Error: El parámetro -S %s no es válido.\n", argv[i]);

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
    if ((m = load_alphabet("alphabet/alphabet.txt", m)) == -1) {
        printf("Error: El alfabeto no se pudo cargar.\n");

        close_files(input, output);
        return -1;
    }

    // Creamos una clave aleatoria para cifrar/descifrar
    if (modo == 0) {
        // Utilizamos srand para tener números aleatorios en cada ejecución del programa
        srand(time(NULL));

        // TODO
    }

    byte = init_byte();
    if (byte == NULL) {
        printf("Error: TODO.\n");

        destroy_alphabet();
        close_files(input, output);
        return -1;
    }

    printf("%s = %d\n", byte, get_byte_value(byte));

    set_byte_to_value(63, byte);

    printf("%s = %d\n", byte,  get_byte_value(byte));

    // Liberamos memoria
    free(byte);

    // Liberamos alfabeto
    destroy_alphabet();

    // Cerramos los archivos si no son stdin/stdout
    close_files(input, output);

    return 0;
}

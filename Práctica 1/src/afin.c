#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <string.h>
#include "euclides.h"

char* alphabet = NULL; // Variable global que contiene el alfabeto

char get_letter(int l) {
    return alphabet[l];
}

int get_letter_code(char c) {
    int i;
    for(i = 0; i < strlen(alphabet); i++){
        if (alphabet[i] == c){
            return i;
        }
    }
    return -1;
}

/**
 * Carga n caracteres del alfabeto en el archivo alphabet/alphabet.txt.
 * Si no hay suficientes caracteres se carga el alfabeto entero.
 * 
 * Devuelve -1 en caso de error y 0 en caso contrario
 */
int load_alphabet(int n) {
    FILE* f = NULL;
    int i, j;

    // Control de errores
    if (n <= 0) return -1;

    // Inicializamos memoria para el alfabeto
    alphabet = (char*) malloc (n*sizeof(char));
    if (alphabet == NULL) return -1;

    // Abrimos el archivo donde está guardado el alfabeto y lo leemos
    f = fopen("alphabet/alphabet.txt", "r");
    if (fgets(alphabet, n+1, f) == NULL) return -1;

    if (n > strlen(alphabet)) {
        n = strlen(alphabet);
        printf("Warning: El tamaño del alfabeto es menor a la n introducida.\n");
        printf("Para cambiar el alfabeto modificar en alphabet/alphabet.txt.\n");
        printf("Se considerará n el tamaño del alfabeto, %d en este caso.\n", n);
    }
    
    for (i = 0; i < strlen(alphabet); i++) {
        for(j = 0; j < i; j++){
            if(alphabet[i] == alphabet[j]){
                printf("Error: El caracter %c está repetido en el alfabeto.\n", alphabet[i]);
                printf("Para cambiar el alfabeto modificar en alphabet/alphabet.txt.\n");

                fclose(f);
                return -1;
            }
        }
    }

    printf("Alfabeto cargado: %s\n", alphabet);

    fclose(f);
    return 0;
}

// Liberamos la memoria del alfabeto
void destroy_alphabet() {
    free(alphabet);
    alphabet = NULL;
}

char affine_char(char origin, int a, int b, int n) {
    origin = get_letter_code(origin);
    if(origin == -1) return '_';
    printf("%d*%d+%d mod %d = %d\n", (origin), a, b, n, (((origin)*a+b)%n));
    return get_letter(((origin)*a+b)%n);
}

void affine_cypher(char* message, mpz_t* inv, mpz_t* a, mpz_t* b, mpz_t* n){
    int i;
    long la, lb, ln;

    // Convertimos la a y la b a positivo si estos están en negativo
    if (mpz_cmp_si(*a, 0) == -1){
        mpz_mod(*a, *a, *n);
    }
    
    if (mpz_cmp_si(*b, 0) == -1){
        mpz_mod(*b, *b, *n);
    }

    get_modular_inverse(inv, a, n);

    if (mpz_cmp_si(*inv, 0) == 0){
        gmp_printf("%Zd no tiene inverso multiplicativo.\n", a);
        return;
    }

    la =  mpz_get_si(*a);
    lb =  mpz_get_si(*b);
    ln =  mpz_get_si(*n);

    for (i=0; i<strlen(message); i++){
        message[i] = affine_char(message[i], la, lb, ln);
    }
}

// Imprime en pantalla el uso de los comandos del programa
void print_help() {
    printf("--------------------------\n");
    printf("./afin {-C|-D} {-m |Zm|} {-a Nx} {-b N+} [-i filein] [-o fileout]\n");
    printf("Siendo los parámetros:\n");
    printf("-C el programa cifra\n");
    printf("-D el programa descifra\n");
    printf("-m tamaño del espacio de texto cifrado\n");
    printf("-a coeficiente multiplicativo de la función afín\n");
    printf("-b término constante de la función afín\n");
    printf("-i fichero de entrada\n");
    printf("-o fichero de salida\n");
    printf("---------------------------\n");
}

int main (int argc, char *argv[]) {
    mpz_t a, b, n, inv;
    char message[100];
    int i, modo = 0; // modo en 0 para cifrar y 1 para descifrar
    FILE *input = stdin, *output = stdout;

    // Inicializamos variables
    mpz_init(a);
    mpz_init(b);
    mpz_init(n);
    mpz_init(inv);

    // Comprobar número de argumentos del usuario
    if (argc < 8 || argc > 12) {
        printf("Error: Número incorrecto de parámetros.\n");
        print_help();
        return -1;
    }

    // Obtener parámetros del usuario
    // afin {-C|-D} {-m |Zm|} {-a Nx} {-b N+} [-i filein] [-o fileout]
    for (i = 1; i < argc; i++) {
        if (strcmp("-C", argv[i]) == 0) {
            // Cifrar
            modo = 0;

        } else if (strcmp("-D", argv[i]) == 0) {
            // Descifrar
            modo = 1;

        } else if (strcmp(argv[i], "-m") == 0) {
            // Tamaño del espacio de texto cifrado
            if (mpz_set_str(n, argv[++i], 10) == -1) {
                printf("Error: El parámetro -m %s no es válido.\n", argv[i]);
                return -1;
            }

        } else if (strcmp(argv[i], "-a") == 0) {
            // Coeficiente multiplicativo de la función afín
            if (mpz_set_str(a, argv[++i], 10) == -1) {
                printf("Error: El parámetro -a %s no es válido.\n", argv[i]);
                return -1;
            }

        } else if (strcmp(argv[i], "-b") == 0) {
            // Término constante de la función afín
            if (mpz_set_str(b, argv[++i], 10) == -1) {
                printf("Error: El parámetro -b %s no es válido.\n", argv[i]);
                return -1;
            }

        // Parámetros de ficheros entrada/salida. Por defecto stdin y stdout
        } else if (strcmp(argv[i], "-i") == 0) {
            // Abrimos fichero de entrada
            input = fopen(argv[++i], "r");
            if (input == NULL) {
                printf("Error: El archivo %s no se ha podido encontrar.\n", argv[i]);
                return -1;
            }

        } else if (strcmp(argv[i], "-o") == 0) {
            // Abrimos fichero de salida
            output = fopen(argv[++i], "w");
            if (output == NULL) {
                printf("Error: El archivo %s no se ha podido encontrar.\n", argv[i]);
                return -1;
            }

        } else {
            printf("Error: Parámetro incorrecto %s.\n", argv[i]);
            print_help();
            return -1;
        }
    }

    // Cargamos el alfabeto del archivo alphabet/alphabet.txt
    if (load_alphabet(mpz_get_si(n)) == -1) {
        printf("Error: El alfabeto no se pudo cargar.\n");
        return -1;
    }
    
    // Leemos el mensaje a cifrar/descifrar
    fscanf(input, "%s", message);

    // Ciframos el mensaje
    affine_cypher(message, &inv, &a, &b, &n);

    // Mostramos el mensaje cifrado/descifrado
    fprintf(output, "El mensaje cifrado es %s\n", message);

    // Liberamos alfabeto
    destroy_alphabet();

    // Cerramos los archivos si no son stdin/stdout
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);

    // Liberamos variables
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(n);
    mpz_clear(inv);

    return 0;
}

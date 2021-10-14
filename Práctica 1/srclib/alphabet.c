#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/alphabet.h"

char* alphabet = NULL; // Variable global que contiene el alfabeto.

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

int load_alphabet(int n) {
    FILE* f = NULL;
    int i, j;

    // Control de errores
    if (n <= 0) return -1;

    // Inicializamos memoria para el alfabeto
    alphabet = (char*) malloc ((n+1) *sizeof(char));
    if (alphabet == NULL) return -1;

    // Abrimos el archivo donde está guardado el alfabeto y lo leemos
    f = fopen("alphabet/alphabet.txt", "r");
    if (fgets(alphabet, n+1, f) == NULL) return -1;

    /* Comprueba que la n introducida no sea mayor que el tamaño del alfabeto en el archivo y si lo es avisa al
    usuario y cambia el valor de n por el tamaño del alfabeto */
    if (n > strlen(alphabet)) {
        n = strlen(alphabet);
        printf("Warning: El tamaño del alfabeto es menor a la n introducida.\n");
        printf("Para cambiar el alfabeto modificar en alphabet/alphabet.txt.\n");
        printf("Se considerará n el tamaño del alfabeto, %d en este caso.\n", n);
    }
    
    // Comprueba si el alfabeto tiene caracteres repetidos
    for (i = 0; i < strlen(alphabet); i++) {
        for (j = 0; j < i; j++) {
            if (alphabet[i] == alphabet[j]) {
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

void destroy_alphabet() {
    // Liberamos la memoria del alfabeto
    free(alphabet);
    alphabet = NULL;
}

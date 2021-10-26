#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/coincidencia.h"
#include "../includes/alphabet.h"

#define MAX_KEY_SIZE 100
#define MAX_TEXT_SIZE 1000000

int main(int argc, char *argv[]) {
    char ciphertext[MAX_TEXT_SIZE], **subStrings = NULL;
    int keyLength, m;
    FILE* fp = fopen("testfiles/test2.txt", "r");
    double averageDeviation;

    // Cargamos el alfabeto del archivo alphabet/alphabet.txt
    m = load_alphabet("alphabet/alphabet.txt", COINCIDENCE_ALPHABET_SIZE);
    if (m == -1 || m != COINCIDENCE_ALPHABET_SIZE) {
        // El alfabeto debe coincidir con el de la tabla de frecuencias
        return -1;
    }

    fgets(ciphertext, MAX_TEXT_SIZE + 1, fp);

    for(keyLength = 1; keyLength < MAX_KEY_SIZE; keyLength++){
        averageDeviation = 0.0;
        subStrings = divide_strings(ciphertext, keyLength);
        for (int i = 0; i < keyLength; i++) {
            averageDeviation += check_key_length(subStrings[i], keyLength);
        }

        printf("El indice de coincidencia para key %d es: %lf\n", keyLength, averageDeviation/keyLength);
        if(averageDeviation/keyLength <= 6.5) break; 
        free_substrings(subStrings, keyLength);
    }

    if(averageDeviation/keyLength >= 6.5){
        printf("No se ha podido encontrar un tamaño válido para la clave\n");
        destroy_alphabet();
        return -1;
    }
    
    printf("El tamaño de la clave es: %d\n", keyLength);

    //subStrings = divide_strings(ciphertext, keyLength);

    for(int i = 0; i < keyLength; i++) {
        printf("%c", get_caesar_key(subStrings[i]));
    }
    printf("\n");

    destroy_alphabet();
    free_substrings(subStrings, keyLength);
    fclose(fp);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/alphabet.h"

#define MINIMUM_COINCIDENCE_SIZE 3

char** get_repeated_strings(char* ciphertext) {
    char** savedStrings = NULL;
    int i, j, len, coincidence, l = 3, stringsFound;
    len = strlen(ciphertext);

    for(i = MINIMUM_COINCIDENCE_SIZE; i < len; i++){
        coincidence = 0;
        for(i = j; j < len; j++){
            if(ciphertext[j] == ciphertext[j+i]){
                coincidence++;
                if(coincidence == MINIMUM_COINCIDENCE_SIZE){
                    //Añadir la string al archivo
                    printf("%c%c%c\n", ciphertext[j-2], ciphertext[j-1], ciphertext[j]);
                }
            } else {
                coincidence = 0;
            }
        }
    }

    return NULL;
}

int main(int argc, char *argv[]){
    char ciphertext[1000000] = "ENTONCESTENEMOSESTASTRDOSINGENLAQUEVAHAHABERCIERTASCOINCIDENCIASDOSSOSPECHOSAS";

    get_repeated_strings(ciphertext);

    return 0;
}
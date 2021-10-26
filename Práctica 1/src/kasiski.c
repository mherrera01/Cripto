#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/alphabet.h"

int main(int argc, char *argv[]){
    char ciphertext[1000000];
    char** savedStrings = NULL;
    int i, j, len, coincidence = 0, l = 3, stringsFound;
    len = strlen(ciphertext);

    for(i = 0; i < len; i++){
        for(i = j; j < len; j++){
            if(ciphertext[i] == ciphertext[j]){
                coincidence++;
                if(coincidence == l){
                    
                }
            }
        }
    }
}
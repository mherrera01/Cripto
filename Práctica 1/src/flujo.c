#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/alphabet.h"

#define MAX_MESSAGE_SIZE 100

int generateRandomNumber(int seed, int m){
    static int generatedNumbers = 0;

    srand(seed+generatedNumbers);
    generatedNumbers++;

    return rand()%m;
}

char encrypt_char(char original, int key, int m){
    return get_letter_code(original) ^ key;
}

char decrypt_char(char original, int key, int m){
    return get_letter(original ^ key);
}

int main (int argc, char *argv[]) {
    int m = DEFAULT_ALPHABET_SIZE;
    int key;
    char message[70] = "Hola miguel :DD";
    char encryptedMessage[70];

    load_alphabet(m);

    printf("Mensaje descifrado:\n");
    for(int i = 0; i < strlen(message); i++){
        key = generateRandomNumber(5233, m);
        encryptedMessage[i] = encrypt_char(message[i], key, m);
        printf("%c", decrypt_char(encryptedMessage[i], key, m));
    }
    printf("\nHasta aquí");

    printf("\n");
    printf("%s\n", encryptedMessage);

    return 0;
}

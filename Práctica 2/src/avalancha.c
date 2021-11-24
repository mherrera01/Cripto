#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../includes/bits.h"
#include "../includes/des.h"

Bits **des_encrypt_round_results(Bits *message, Bits *key) {
    Bits **subkeys = NULL;
    Bits **roundResult = NULL;
    Bits *roundMsg = NULL;
    int round = 0;

    // Control de errores
    if (message == NULL || key == NULL) return NULL;
    if (get_bits_size(message) != 64 || get_bits_size(key) != 64) return NULL;

    // Obtenemos las subclaves para cifrar
    subkeys = get_des_subkeys(key);
    if (subkeys == NULL) return NULL;

    roundResult = (Bits**) malloc (17 * sizeof(Bits*));
    if(roundResult == NULL){
        free_des_subkeys(subkeys);
        return NULL;
    }

    // Permutamos el texto plano inicial
    roundMsg = initial_permutation(message);
    if (roundMsg == NULL) {
        free_des_subkeys(subkeys);
        free(roundResult);
        return NULL;
    }

    // Des tiene 16 rondas.
    while (round < 16) {
        // Ejecutamos la ronda con la clave
        if (compute_round(roundMsg, subkeys[round]) == -1) {
            destroy_bits(roundMsg);
            free_des_subkeys(subkeys);
            free(roundResult);
            return NULL;
        }

        roundResult[round] = copy_bits(roundMsg);
        round++; // Siguiente ronda
    }

    // Obtenemos el mensaje cifrado a partir de la salida de la última ronda
    roundResult[16] = compute_last_round(roundMsg); // TODO

    // Liberamos memoria
    destroy_bits(roundMsg);
    free_des_subkeys(subkeys);

    return roundResult;
}

int main(){
    Bits *key=NULL, *modifiedKey=NULL, *plaintext=NULL, *modifiedPlaintext=NULL;
    Bits **controlResults=NULL, **keyResults=NULL, **plaintextResults=NULL;
    int i, j, plaintextDifference=0, keyDifference=0;

    srand(time(NULL));

    if (set_bits_desKey_value(key) == -1){
        return -1;
    }
    if (set_bits_desKey_value(modifiedKey) == -1){
        destroy_bits(key);
        return -1;
    }
    if (set_bits_desKey_value(plaintext) == -1){
        destroy_bits(key);
        destroy_bits(modifiedKey);
        return -1;
    }
    if (set_bits_desKey_value(modifiedPlaintext) == -1){
        destroy_bits(key);
        destroy_bits(modifiedKey);
        destroy_bits(plaintext);
        return -1;
    }

    i = rand() % 64;

    if(get_bit(modifiedKey, i) == ASCII_0) set_bit(modifiedKey, i, ASCII_1);
    else set_bit(modifiedKey, i, ASCII_0);

    if(get_bit(modifiedPlaintext, i) == ASCII_0) set_bit(modifiedPlaintext, i, ASCII_1);
    else set_bit(modifiedPlaintext, i, ASCII_0);

    controlResults = des_encrypt_round_results(plaintext, key);
    keyResults = des_encrypt_round_results(plaintext, modifiedKey);
    plaintextResults = des_encrypt_round_results(modifiedPlaintext, key);

    for(i = 0; i < 17; i++){
        for(j = 0; j < 64; j++){
            if(get_bit(keyResults[i], j) != get_bit(controlResults[i], j)) keyDifference++;
            if(get_bit(plaintextResults[i], j) != get_bit(controlResults[i], j)) plaintextDifference++;
        }
        if(i!=16){
            printf("En la ronda %d, el mensaje con un bit cambiado en el texto plano tiene %d bits diferentes.\n", i, plaintextDifference);
            printf("En la ronda %d, el mensaje con un bit cambiado en la clave tiene %d bits diferentes.\n", i, keyDifference);
        } else {
            printf("El mensaje final cifrado con un bit cambiado en el texto plano tiene %d bits diferentes.\n", plaintextDifference);
            printf("El mensaje final cifrado con un bit cambiado en la clave tiene %d bits diferentes.\n", keyDifference);
        }
    }

    return 0;
}
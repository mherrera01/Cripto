#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../includes/bits.h"
#include "../includes/des.h"

#define ROUND_RESULTS_SIZE 17

// Libera la memoria de los resultados del mensaje de cada ronda
void free_round_results(Bits **roundResult) {
    int i;

    if (roundResult != NULL) {
        for (i = 0; i < ROUND_RESULTS_SIZE; i++) {
            if (roundResult[i] != NULL) destroy_bits(roundResult[i]);
        }
        free(roundResult);
    }
}

// Devuelve los resultados del mensaje de cada ronda o NULL en caso de error.
Bits **des_encrypt_round_results(Bits *message, Bits *key) {
    Bits **subkeys = NULL, **roundResult = NULL;
    Bits *roundMsg = NULL;
    int round = 0;

    // Control de errores
    if (message == NULL || key == NULL) return NULL;
    if (get_bits_size(message) != 64 || get_bits_size(key) != 64) return NULL;

    // Obtenemos las subclaves para cifrar
    subkeys = get_des_subkeys(key);
    if (subkeys == NULL) return NULL;

    // Reservamos memoria para los resultados de cada ronda
    roundResult = (Bits**) calloc (sizeof(Bits*), ROUND_RESULTS_SIZE);
    if (roundResult == NULL) {
        free_des_subkeys(subkeys);
        return NULL;
    }

    // Permutamos el texto plano inicial
    roundMsg = initial_permutation(message);
    if (roundMsg == NULL) {
        free_round_results(roundResult);
        free_des_subkeys(subkeys);
        return NULL;
    }

    // Des tiene 16 rondas.
    while (round < 16) {
        // Ejecutamos la ronda con la clave
        if (compute_round(roundMsg, subkeys[round]) == -1) {
            destroy_bits(roundMsg);
            free_round_results(roundResult);
            free_des_subkeys(subkeys);
            return NULL;
        }

        // Guardamos el resultado de la ronda
        roundResult[round] = copy_bits(roundMsg);
        if (roundResult[round] == NULL) {
            destroy_bits(roundMsg);
            free_round_results(roundResult);
            free_des_subkeys(subkeys);
            return NULL;
        }

        round++; // Siguiente ronda
    }

    // Obtenemos el mensaje cifrado a partir de la salida de la última ronda
    roundResult[16] = compute_last_round(roundMsg);
    if (roundResult[16] == NULL) {
        destroy_bits(roundMsg);
        free_round_results(roundResult);
        free_des_subkeys(subkeys);
        return NULL;
    }

    // Liberamos memoria
    destroy_bits(roundMsg);
    free_des_subkeys(subkeys);

    return roundResult;
}

// Libera la memoria de los bits inicializados
void free_bits_mem (Bits *key, Bits *modifiedKey, Bits *plaintext, Bits *modifiedPlaintext) {
    if (key != NULL) destroy_bits(key);
    if (modifiedKey != NULL) destroy_bits(modifiedKey);
    if (plaintext != NULL) destroy_bits(plaintext);
    if (modifiedPlaintext != NULL) destroy_bits(modifiedPlaintext);
}

// Libera la memoria de las listas de bits de las rondas inicializadas
void free_rounds_bits_mem (Bits **controlResults, Bits **keyResults, Bits **plaintextResults) {
    if (controlResults != NULL) free_round_results(controlResults);
    if (keyResults != NULL) free_round_results(keyResults);
    if (plaintextResults != NULL) free_round_results(plaintextResults);
}

int main(int argc, char *argv[]) {
    Bits *key = NULL, *modifiedKey = NULL, *plaintext = NULL, *modifiedPlaintext = NULL;
    Bits **controlResults = NULL, **keyResults = NULL, **plaintextResults = NULL;
    int i, j, plaintextDifference = 0, keyDifference = 0;

    // Utilizamos srand para tener números aleatorios en cada ejecución del programa
    srand(time(NULL));

    // Creamos números aleatorios de 64 bits
    key = generate_random_desKey();
    modifiedKey = generate_random_desKey();
    plaintext = generate_random_desKey();
    modifiedPlaintext = generate_random_desKey();

    // Control de errores
    if (key == NULL || modifiedKey == NULL || plaintext == NULL || modifiedPlaintext == NULL) {
        free_bits_mem(key, modifiedKey, plaintext, modifiedPlaintext);
        return -1;
    }

    // Número aleatorio entre 0-63
    i = rand() % 64;

    if (get_bit(modifiedKey, i) == ASCII_0) set_bit(modifiedKey, i, ASCII_1);
    else set_bit(modifiedKey, i, ASCII_0);

    if (get_bit(modifiedPlaintext, i) == ASCII_0) set_bit(modifiedPlaintext, i, ASCII_1);
    else set_bit(modifiedPlaintext, i, ASCII_0);

    controlResults = des_encrypt_round_results(plaintext, key);
    keyResults = des_encrypt_round_results(plaintext, modifiedKey);
    plaintextResults = des_encrypt_round_results(modifiedPlaintext, key);

    // Control de errores
    if (controlResults == NULL || keyResults == NULL || plaintextResults == NULL) {
        free_rounds_bits_mem(controlResults, keyResults, plaintextResults);
        free_bits_mem(key, modifiedKey, plaintext, modifiedPlaintext);
        return -1;
    }

    for (i = 0; i < ROUND_RESULTS_SIZE; i++) {
        for (j = 0; j < 64; j++) {
            if (get_bit(keyResults[i], j) != get_bit(controlResults[i], j)) keyDifference++;
            if (get_bit(plaintextResults[i], j) != get_bit(controlResults[i], j)) plaintextDifference++;
        }
        if (i != ROUND_RESULTS_SIZE - 1) {
            printf("En la ronda %d, el mensaje con un bit cambiado en el texto plano tiene %d bits diferentes.\n", i, plaintextDifference);
            printf("En la ronda %d, el mensaje con un bit cambiado en la clave tiene %d bits diferentes.\n", i, keyDifference);
        } else {
            printf("El mensaje final cifrado con un bit cambiado en el texto plano tiene %d bits diferentes.\n", plaintextDifference);
            printf("El mensaje final cifrado con un bit cambiado en la clave tiene %d bits diferentes.\n", keyDifference);
        }
    }

    // Liberamos memoria
    free_rounds_bits_mem(controlResults, keyResults, plaintextResults);
    free_bits_mem(key, modifiedKey, plaintext, modifiedPlaintext);

    return 0;
}

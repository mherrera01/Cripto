#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../includes/des.h"
#include "../includes/byteArray.h"
#include "../includes/DES_tables.h"

/**
 * Permuta y comprime la clave inicial, eliminando bits de paridad.
 * 
 * Devuelve -1 en caso de error y 0 en caso contrario.
 */
int permuted_choice_one(ByteArray *key) {
    // Control de errores
    if (key == NULL) return -1;

    // PC1[]

    return 0;
}

/**
 * Permuta y comprime para obtener cada subclave.
 * 
 * Devuelve -1 en caso de error y 0 en caso contrario.
 */
int permuted_choice_two(ByteArray *key) {
    // Control de errores
    if (key == NULL) return -1;

    // PC2[]

    return 0;
}

/**
 * Rotación hacia la izquierda (1 ó 2 bits según la ronda) en
 * la ronda pasada como argumento.
 * 
 * Devuelve -1 en caso de error y 0 en caso contrario.
 */ 
int left_circular_shift(ByteArray *key, int round) {
    // Control de errores
    if (key == NULL || round < 0 || round > 15) return -1;

    // ROUND_SHIFTS[round]

    return 0;
}

int compute_round(ByteArray *message, ByteArray *key) {
    // Control de errores
    if (message == NULL || key == NULL) return -1;

    return 0;
}

int des_encrypt(ByteArray *message, ByteArray *key) {
    int round = 0;

    // Control de errores
    if (message == NULL || key == NULL) return -1;

    // permuted_choice_one(key);

    // Dividimos la clave en dos partes de 28 bits

    // left_circular_shift(key1, round);
    // left_circular_shift(key2, round);

    // Volvemos a juntar la clave en 56 bits

    // permuted_choice_two(key);

    return 0;
}

int des_decrypt(ByteArray *key) {
    // Control de errores
    if (key == NULL) return -1;

    return 0;
}

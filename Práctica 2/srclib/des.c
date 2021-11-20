#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../includes/byteArray.h"
#include "../includes/des.h"
#include "../includes/DES_tables.h"

ByteArray *generate_random_desKey() {
    ByteArray *key = NULL;

    // Inicializamos una lista de 8 bytes (64 bits)
    key = init_byteArray(8);
    if (key == NULL) return NULL;

    // Asignamos un valor aleatorio a la clave con sus correspondientes bits de paridad
    if (set_byteArray_desKey_value(key) == -1) {
        destroy_byteArray(key);
        return NULL;
    }

    return key;
}

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

ByteArray *des_encrypt(ByteArray *message, ByteArray *key) {
    ByteArray *encrypted = NULL;
    // int round = 0;

    // Control de errores
    if (message == NULL || key == NULL) return NULL;
    if (get_byteArray_size(message) != 8 || get_byteArray_size(key) != 8) return NULL;

    // permuted_choice_one(key);

    // Dividimos la clave en dos partes de 28 bits

    // left_circular_shift(key1, round);
    // left_circular_shift(key2, round);

    // Volvemos a juntar la clave en 56 bits

    // permuted_choice_two(key);

    return encrypted;
}

int des_decrypt(ByteArray *key) {
    // Control de errores
    if (key == NULL) return -1;

    return 0;
}

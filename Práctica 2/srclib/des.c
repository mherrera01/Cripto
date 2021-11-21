#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../includes/bits.h"
#include "../includes/des.h"
#include "../includes/DES_tables.h"

Bits *generate_random_desKey() {
    Bits *key = NULL;

    // Inicializamos una lista de 64 bits (8 bytes)
    key = init_bits(64);
    if (key == NULL) return NULL;

    // Asignamos un valor aleatorio a la clave con sus correspondientes bits de paridad
    if (set_bits_desKey_value(key) == -1) {
        destroy_bits(key);
        return NULL;
    }

    return key;
}

/**
 * Permutación inicial de los 64 bits de entrada.
 * 
 * Devuelve NULL en caso de error o la clave permutada en caso contrario.
 */
Bits *initial_permutation(Bits *message) {
    Bits *permutedMsg = NULL;
    int i;

    // Control de errores
    if (message == NULL || get_bits_size(message) != 64) return NULL;

    // Inicializamos memoria para la permutación de 64 bits
    permutedMsg = init_bits(BITS_IN_IP);
    if(!permutedMsg) return NULL;

    // Permutamos los bits de la clave
    for(i = 0; i < BITS_IN_IP; i++){
        set_bit(permutedMsg, i, get_bit(message, IP[i]-1));
    }

    return permutedMsg;
}

/**
 * Inversa de la permutación inicial, para obtener los
 * 64 bits de salida.
 * 
 * Devuelve NULL en caso de error o la clave permutada en caso contrario.
 */
Bits* inverse_initial_permutation(Bits *message) {
    Bits *invPermutedMsg = NULL;
    int i;

    // Control de errores
    if (message == NULL || get_bits_size(message) != 64) return NULL;

    // Inicializamos memoria para la permutación inversa de 64 bits
    invPermutedMsg = init_bits(BITS_IN_IP);
    if(!invPermutedMsg) return NULL;

    // Permutamos los bits de la clave
    for(i = 0; i < BITS_IN_IP; i++){
        set_bit(invPermutedMsg, i, get_bit(message, IP_INV[i]-1));
    }

    return invPermutedMsg;
}

/**
 * Permuta y comprime la clave inicial, eliminando bits de paridad.
 * 
 * Devuelve NULL en caso de error o la clave permutada en caso contrario.
 */
Bits *permuted_choice_one(Bits *key) {
    Bits *permutedKey = NULL;
    int i;

    // Control de errores
    if (key == NULL || get_bits_size(key) != 64) return NULL;

    // Inicializamos memoria para la clave comprimida de 56 bits
    permutedKey = init_bits(BITS_IN_PC1);
    if(!permutedKey) return NULL;

    // Permutamos los bits de la clave
    for(i = 0; i < BITS_IN_PC1; i++){
        set_bit(permutedKey, i, get_bit(key, PC1[i]-1));
    }

    return permutedKey;
}

/**
 * Permuta y comprime para obtener cada subclave.
 * 
 * Devuelve NULL en caso de error o la clave permutada en caso contrario.
 */
Bits *permuted_choice_two(Bits *key) {
    Bits *permutedKey = NULL;
    int i;

    // Control de errores
    if (key == NULL || get_bits_size(key) != 56) return NULL;

    // Inicializamos memoria para la clave comprimida de 48 bits
    permutedKey = init_bits(BITS_IN_PC2);
    if(!permutedKey) return NULL;

    // Permutamos los bits de la clave
    for(i = 0; i < BITS_IN_PC2; i++){
        set_bit(permutedKey, i, get_bit(key, PC2[i]-1));
    }

    return permutedKey;
}

/**
 * Rotación hacia la izquierda (1 ó 2 bits según la ronda) en
 * la ronda pasada como argumento.
 * 
 * Devuelve -1 en caso de error o 0 en caso contrario.
 */ 
int left_circular_shift(Bits *key, int round) {
    int i, j, shifts;
    char *overflowBits = NULL;

    // Control de errores
    if (key == NULL || get_bits_size(key) != 28 || round < 0 || round > 15) return -1;

    // Obtenemos el número de rotaciones necesarios en la ronda
    shifts = ROUND_SHIFTS[round];
    if (shifts < 0) return -1;
    if (shifts == 0) return 0;

    // Reservamos memoria para los bits que excedan al hacer la rotación
    overflowBits = (char *) malloc (sizeof(char) * shifts);
    if (overflowBits == NULL) return -1;

    // Hallamos los bits que exceden en la rotación a la izquierda
    for (i = 0; i < shifts; i++) {
        overflowBits[i] = get_bit(key, i);
    }

    // Rotamos a la izquierda la clave de 28 bits
    for (i = shifts; i < 28; i++) {
        set_bit(key, i - shifts, get_bit(key, i));
    }

    // Asignamos los bits excedidos a la derecha
    for (i = 28 - shifts, j = 0; i < 28; i++, j++) {
        set_bit(key, i, overflowBits[j]);
    }

    // Liberamos memoria
    free(overflowBits);

    return 0;
}

// Métodos de la ronda -------------

/**
 * Permutación y expansión de los bits de R(i-1).
 * 
 * Devuelve NULL en caso de error o la clave permutada en caso contrario.
 */
Bits *expansion_permutation(Bits *messageR) {
    Bits *permutedMsg = NULL;
    int i;

    // Control de errores
    if (messageR == NULL || get_bits_size(messageR) != 32) return NULL;

    // Inicializamos memoria para la permutación de 48 bits
    permutedMsg = init_bits(BITS_IN_E);
    if(!permutedMsg) return NULL;

    // Expandimos de 32 bits a 48 el mensaje dividido R
    for(i = 0; i < BITS_IN_E; i++){
        set_bit(permutedMsg, i, get_bit(messageR, E[i]-1));
    }

    return permutedMsg;
}

Bits *sustitution_box(Bits *xorMessage) {
    Bits *sboxMsg = NULL, *sboxRow = NULL, *sboxColumn = NULL;
    int i, j, x, rowBits, columnBits, startIndex;

    // Control de errores
    if (xorMessage == NULL || get_bits_size(xorMessage) != 48) return NULL;

    // Inicializamos memoria para el resultado de las cajas s de 32 bits
    sboxMsg = init_bits(NUM_S_BOXES * ROWS_PER_SBOX);
    if(!sboxMsg) return NULL;

    // Calculamos cuántos bits son necesarios para las filas (2 bits) y columnas (4 bits) de cada caja s
    rowBits = log(ROWS_PER_SBOX) / log(2);
    columnBits = log(COLUMNS_PER_SBOX) / log(2);

    // Cada una de las ocho cajas s permutan y comprimen de 6 a 4 bits
    for (i = 0; i < NUM_S_BOXES; i++) {
        // Obtenemos la fila de la caja Si
        sboxRow = init_bits(rowBits);
        if (sboxRow == NULL) {
            destroy_bits(sboxMsg);
            return NULL;
        }

        // a1 a2 a3 a4 a5 a6 -> a1 a6 es la fila
        startIndex = i*(columnBits + rowBits);
        set_bit(sboxRow, 0, get_bit(xorMessage, startIndex));
        set_bit(sboxRow, 1, get_bit(xorMessage, (i+1)*(columnBits + rowBits)));

        // Obtenemos la columna de la caja Si
        sboxColumn = init_bits(columnBits);
        if (sboxColumn == NULL) {
            destroy_bits(sboxRow);
            destroy_bits(sboxMsg);
            return NULL;
        }

        // a1 a2 a3 a4 a5 a6 -> a2 a3 a4 a5 es la columna
        for (j = startIndex + 1, x = 0; j < startIndex + 1 + columnBits; j++, x++) {
            set_bit(sboxColumn, x, get_bit(xorMessage, j));
        }

        // Asignamos el resultado de la caja Si
        set_hex_to_value(sboxMsg, i, S_BOXES[i][get_bits_decimal_value(sboxRow)][get_bits_decimal_value(sboxColumn)]); 

        // Liberamos memoria
        destroy_bits(sboxRow);
        destroy_bits(sboxColumn);
    }

    return sboxMsg;
}

/**
 * Permutación de la salida de las cajas de sustitución.
 * 
 * Devuelve NULL en caso de error o la clave permutada en caso contrario.
 */
Bits *permutation_function(Bits *messageSBox) {
    Bits *permutedMsg = NULL;
    int i;

    // Control de errores
    if (messageSBox == NULL || get_bits_size(messageSBox) != 32) return NULL;

    // Inicializamos memoria para la permutaciṕn de 32 bits
    permutedMsg = init_bits(32);
    if(!permutedMsg) return NULL;

    // Permutamos el mensaje de las cajas de sustitución
    for(i = 0; i < 32; i++){
        set_bit(permutedMsg, i, get_bit(messageSBox, P[i]-1));
    }

    return permutedMsg;
}

Bits *compute_fRound(Bits *messageR, Bits *key) {
    Bits *expandedMessageR = NULL, *xorMessageR = NULL, *sboxMessageR = NULL, *permutedSBox = NULL;

    // Control de errores
    if (messageR == NULL || key == NULL) return NULL;
    if (get_bits_size(messageR) != 32 || get_bits_size(key) != 48) return NULL;

    // Expandimos el mensaje dividido R a 48 bits
    expandedMessageR = expansion_permutation(messageR);
    if (expandedMessageR == NULL) return NULL;
    
    // XOR del mensaje R expandido y la clave de 48 bits
    xorMessageR = xor_bits(expandedMessageR, key);
    if (xorMessageR == NULL) {
        destroy_bits(expandedMessageR);
        return NULL;
    }

    // Permutamos en las cajas S y comprimimos el mensaje R a 32 bits
    sboxMessageR = sustitution_box(xorMessageR);

    // Permutamos la salida de las cajas de sustitución
    permutedSBox = permutation_function(sboxMessageR);

    // Liberamos memoria
    destroy_bits(sboxMessageR);
    destroy_bits(xorMessageR);
    destroy_bits(expandedMessageR);

    return permutedSBox;
}

/**
 * Calcula el resultado de una ronda dada una clave y lo devuelve
 * en el mensaje 
 * 
 * Devuelve -1 en caso de error 0 en caso contrario.
 */
int compute_round(Bits *message, Bits *key) {
    Bits *messageL = NULL, *messageR = NULL, *fResult = NULL;
    Bits *nextMessageR = NULL, *nextMessage = NULL;

    // Control de errores
    if (message == NULL || key == NULL) return -1;
    if (get_bits_size(message) != 64 || get_bits_size(key) != 48) return -1;

    // Dividimos el mensaje en dos partes de 32 bits
    if (split_bits(message, 32, &messageL, &messageR) == -1) return -1;

    // El mensaje R es pasado como argumento a la función F junto con la clave
    fResult = compute_fRound(messageR, key);
    if (fResult == NULL) {
        destroy_bits(messageL);
        destroy_bits(messageR);
        return -1;
    }

    // Calculamos la operación XOR de L(i-1) y el resultado de la función F
    nextMessageR = xor_bits(messageL, fResult);
    if (nextMessageR == NULL) {
        destroy_bits(fResult);
        destroy_bits(messageL);
        destroy_bits(messageR);
        return -1;
    }

    // Obtenemos la salida de la ronda
    nextMessage = merge_bits(messageR, nextMessageR);

    // Asignamos el resultado al mensaje dado inicialmente
    set_bits_decimal_value(message, get_bits_decimal_value(nextMessage));

    // Liberamos memoria
    destroy_bits(nextMessage);
    destroy_bits(nextMessageR);
    destroy_bits(fResult);
    destroy_bits(messageL);
    destroy_bits(messageR);

    return 0;
}

Bits *des_encrypt(Bits *message, Bits *key) {
    Bits *encryptedMsg = NULL, *roundMsg = NULL, *swap1 = NULL, *swap2 = NULL;
    Bits *roundKey = NULL, *roundKeyPermuted = NULL, *key1 = NULL, *key2 = NULL;
    int round = 0;

    // Control de errores
    if (message == NULL || key == NULL) return NULL;
    if (get_bits_size(message) != 64 || get_bits_size(key) != 64) return NULL;

    // Permutamos la clave y la convertimos a 56 bits
    roundKey = permuted_choice_one(key);
    if (roundKey == NULL) return NULL;

    // Permutamos el texto plano inicial
    roundMsg = initial_permutation(message);
    if (roundMsg == NULL) {
        destroy_bits(roundKey);
        return NULL;
    }

    // Des tiene 16 rondas.
    while (round < 16) {
        // Dividimos la clave en dos partes de 28 bits.
        if (split_bits(roundKey, 28, &key1, &key2) == -1) {
            destroy_bits(roundMsg);
            destroy_bits(roundKey);
            return NULL;
        }

        // Rotamos hacia la izquierda la clave dividida.
        if (left_circular_shift(key1, round) == -1 || left_circular_shift(key2, round) == -1) {
            destroy_bits(key1);
            destroy_bits(key2);
            destroy_bits(roundMsg);
            destroy_bits(roundKey);
            return NULL;
        }

        // Volvemos a juntar la clave en 56 bits.
        destroy_bits(roundKey);
        roundKey = merge_bits(key1, key2);

        // Permutamos la clave y la comvertimos en 48 bits 
        roundKeyPermuted = permuted_choice_two(roundKey);
        if (roundKeyPermuted == NULL) {
            destroy_bits(key1);
            destroy_bits(key2);
            destroy_bits(roundMsg);
            destroy_bits(roundKey);
            return NULL;
        }

        // Ejecutamos la ronda
        if (compute_round(roundMsg, roundKeyPermuted) == -1) {
            destroy_bits(roundKeyPermuted);
            destroy_bits(key1);
            destroy_bits(key2);
            destroy_bits(roundMsg);
            destroy_bits(roundKey);
            return NULL;
        }

        // Liberamos memoria
        destroy_bits(roundKeyPermuted);
        destroy_bits(key1);
        destroy_bits(key2);

        round++; // Siguiente ronda
    }

    // Swap de 32 bits
    if (split_bits(roundMsg, 32, &swap1, &swap2) == -1) {
        destroy_bits(roundMsg);
        destroy_bits(roundKey);
        return NULL;
    }

    destroy_bits(roundMsg);
    roundMsg = merge_bits(swap1, swap2);
    if (roundMsg == NULL) {
        destroy_bits(swap1);
        destroy_bits(swap2);
        return NULL;
    }

    // Inversa de la permutación inicial
    encryptedMsg = inverse_initial_permutation(roundMsg);

    // Liberamos memoria
    destroy_bits(swap1);
    destroy_bits(swap2);
    destroy_bits(roundMsg);
    destroy_bits(roundKey);

    return encryptedMsg;
}

int des_decrypt(Bits *key) {
    // Control de errores
    if (key == NULL) return -1;

    return 0;
}

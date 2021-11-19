#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../includes/byte.h"

/**
 * Comprueba si todos los bits del byte están en binario.
 * 
 * Devuelve 0 si el byte está en binario o -1 en caso contrario.
 */
int check_byte(char *byte) {
    int i;

    // Control de errores
    if (byte == NULL) return -1;

    // Iteramos por los bits del byte
    for (i=0; i<BYTE_SIZE; i++) {
        // Comprobamos si el bit es correcto {0,1}
        if (byte[i] != '0' && byte[i] != '1') {
            return -1;
        }
    }

    return 0;
}

/**
 * Se suman los bits cuyo valor es uno, si da un número impar de bits, entonces el
 * bit de paridad (impar) es cero. Y si la suma de los bits cuyo valor es uno es par,
 * entonces el bit de paridad (impar) se establece en uno, haciendo impar la cuenta
 * total de bits uno.
 * 
 * Devuelve -1 en caso de error o 0 en caso contrario.
 */
int set_odd_parity_byte(char *byte) {
    int i, counter = 0;

    // Control de errores
    if (byte == NULL) return -1;

    // Iteramos por los bits del byte
    for (i = 0; i < BYTE_SIZE-1; i++) {
        if (byte[i] == '1') {
            counter++;
        }
    }

    // Asignamos el bit de paridad impar
    if (counter % 2 == 0) {
        byte[BYTE_SIZE-1] = '1';
    } else {
        byte[BYTE_SIZE-1] = '0';
    }

    return 0;
}

char* init_byte(){
    char *byte = NULL;

    // Reservamos memoria para BYTE_SIZE bits en el byte
    byte = (char*) malloc (BYTE_SIZE*sizeof(char));
    if (byte == NULL) return NULL;

    // Inicializamos el byte a 0
    memset(byte, ASCII_0, BYTE_SIZE);

    // Comprobamos si el byte está en notación binaria
    if (check_byte(byte) == -1) {
        free(byte);
        return NULL;
    }

    return byte;
}

int set_byte_to_value(char* byte, unsigned char value){
    int i, j;

    // Control de errores
    if (byte == NULL) return -1;

    // Comprobamos si el byte está en notación binaria
    if (check_byte(byte) == -1) return -1;

    // Transformamos de decimal a binario
    for (i = 1<<(BYTE_SIZE-1), j = 0; i > 0; i=i/2, j++) {
        if (value >= i) {
            byte[j] = '1';
            value -= i;
        } else {
            byte[j] = '0';
        }
    }

    return 0;
}

unsigned char get_byte_value(char* byte){
    int i, j;
    unsigned char value = 0;

    // Control de errores
    if (byte == NULL) return 0;

    // Comprobamos si el byte está en notación binaria
    if (check_byte(byte) == -1) return 0;

    // Transformamos de binario a decimal
    for (i = 1<<(BYTE_SIZE-1), j = 0; i > 0; i=i/2, j++) {
        if (byte[j] == '1') {
            value += i;
        }
    }

    return value;
}

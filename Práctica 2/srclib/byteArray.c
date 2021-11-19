#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../includes/byteArray.h"
#include "../includes/byte.h"

// Estructura que guarda una lista de bytes
struct _ByteArray {
    int size; // Número de bytes
    char** bytes; // Valores de los bytes. Posición 0 corresponde al byte más significativo
};

void destroy_byteArray(ByteArray *byteArray) {
    int i;

    if (byteArray == NULL) return;

    if (byteArray->bytes != NULL) {
        for (i = 0; i < byteArray->size; i++) {
            if (byteArray->bytes[i] != NULL) {
                // Liberamos la memoria del byte
                free(byteArray->bytes[i]);
                byteArray->bytes[i] = NULL;
            }
        }

        // Liberamos la memoria de la lista de bytes
        free(byteArray->bytes);
    }  

    // Liberamos la memoria de la estructura
    free(byteArray);
}

ByteArray *init_byteArray(int size) {
    int i;
    ByteArray *byteArray = NULL;

    // Control de errores.
    if (size <= 0) return NULL;

    // Reservamos memoria para la lista de bytes.
    byteArray = (ByteArray*) malloc (sizeof(ByteArray));
    if (byteArray == NULL) return NULL;
    
    // Inicializamos los valores de la estructura.
    byteArray->size = size;
    byteArray->bytes = (char**) malloc (size*sizeof(char*));
    if (byteArray->bytes == NULL) {
        destroy_byteArray(byteArray);
        return NULL;
    }

    // Reservamos memoria para los valores de los bytes.
    for (i = 0; i < size; i++) {
        byteArray->bytes[i] = init_byte();
        if (byteArray->bytes[i] == NULL) {
            destroy_byteArray(byteArray);
            return NULL;
        }
    }

    return byteArray;
}

int set_byteArray_to_value(ByteArray *byteArray, unsigned long long value) {
    int i, j;

    // Control de errores.
    if (byteArray == NULL || byteArray->bytes == NULL) return -1;

    // Para cada byte ponemos el valor de la variable value desplazada i bytes a la derecha.
    for (i = byteArray->size; i > 0; i--) {
        if (byteArray->bytes[byteArray->size-i] == NULL) return -1;
        set_byte_to_value(byteArray->bytes[byteArray->size-i], value>>(i-1)*BYTE_SIZE);
    }

    return 0;
}

unsigned long long get_byteArray_value(ByteArray *byteArray) {
    int i;
    unsigned long long value = 0LL;
    
    // Control de errores.
    if (byteArray == NULL || byteArray->bytes == NULL) return 0LL;

    // Sumamos el valor de cada byte al value y después multiplicamos el value por 2^8 antes de sumarle el siguiente byte.
    for (i=0; i<byteArray->size; i++) {
        if (byteArray->bytes[i] == NULL) return 0LL;
        value = value<<BYTE_SIZE;
        value += get_byte_value(byteArray->bytes[i]);
    }

    return value;
}

int print_byteArray(ByteArray *byteArray) {
    int i;

    // Control de errores.
    if (byteArray == NULL || byteArray->bytes == NULL) return -1;

    // Mostramos el número de bytes.
    printf("Número de bytes: %d\n", byteArray->size);

    // Mostramos los valores de los bytes.
    printf("Resultado en notación binaria:\n");
    for (i = 0; i < byteArray->size; i++) {
        // En caso de que no se haya inicializado alguno de los valores, mostramos un error.
        if (byteArray->bytes[i] == NULL) {
            return -1;
        }

        // Mostramos el byte en binario.
        printf("%.*s\n", BYTE_SIZE, byteArray->bytes[i]);
    }

    return 0;
}

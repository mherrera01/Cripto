#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
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

// Setters -------------
int set_byteArray_desKey_value(ByteArray *byteArray) {
    int i;
    unsigned char random;

    // Control de errores.
    if (byteArray == NULL || byteArray->bytes == NULL) return -1;

    // Utilizamos srand para tener números aleatorios en cada ejecución del programa
    srand(time(NULL));

    for (i = 0; i < byteArray->size; i++) {
        // Generamos un número positivo aleatorio de 7 bits
        random = rand()%128;

        // Asignamos el valor al byte
        if (set_byte_desKey_value(byteArray->bytes[i], random) == -1) return -1;
    }

    return 0;
}

int set_byteArray_to_value(ByteArray *byteArray, unsigned long long value) {
    int i;

    // Control de errores.
    if (byteArray == NULL || byteArray->bytes == NULL) return -1;

    // Para cada byte ponemos el valor de la variable value desplazada i bytes a la derecha.
    for (i = byteArray->size; i > 0; i--) {
        if (set_byte_to_value(byteArray->bytes[byteArray->size-i], value>>(i-1)*BYTE_SIZE) == -1) return -1;
    }

    return 0;
}

int set_byteArray_byte(ByteArray* byteArray, char* byte, int index) {
    // Control de errores.
    if(!byteArray || index < 0 || !byte) return -1;
    if(byteArray->size <= index || byteArray->bytes == NULL) return -1;
    
    // Ponemos el mismo valor al byte del array que al byte.
    set_byte_to_value(byteArray->bytes[index], get_byte_value(byte));

    return 0;
}

int set_byteArray_byte_value(ByteArray* byteArray, unsigned char byteValue, int index) {
    // Control de errores.
    if(!byteArray || index < 0) return -1;
    if(byteArray->size <= index || byteArray->bytes == NULL) return -1;
    
    // Ponemos el mismo valor al byte del array que al byte.
    set_byte_to_value(byteArray->bytes[index], byteValue);

    return 0;
}

// Getters -------------
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

char* get_byteArray_byte(ByteArray* byteArray, int index) {
    char* byte = NULL;

    // Control de errores.
    if(!byteArray || index < 0) return NULL;
    if(byteArray->size <= index || byteArray->bytes == NULL) return NULL;

    // Reservamos memoria para el byte a devolver.
    byte = init_byte();
    if(byte == NULL) return NULL;
    
    // Ponemos el mismo valor al byte generado del que tiene el byte del array.
    set_byte_to_value(byte, get_byte_value(byteArray->bytes[index]));

    return byte;
}

unsigned char get_byteArray_byte_value(ByteArray* byteArray, int index) {
    // Control de errores.
    if(!byteArray || index < 0) return 0;
    if(byteArray->size <= index || byteArray->bytes == NULL) return 0;

    return get_byte_value(byteArray->bytes[index]);
}

int get_byteArray_size(ByteArray* byteArray){
    if(!byteArray) return -1;

    return byteArray->size;
}

// Misceláneo ----------------
int split_byteArray(ByteArray* original, int split, ByteArray* part1, ByteArray* part2){
    int i;

    // Control de errores. 
    if(!original || split < 1) return -1;
    if(!original->bytes || original->size <= split) return -1;

    // Inicializamos la memoria de los sub arrays.
    part1 = init_byteArray(split);
    if (part1 == NULL){
        return -1;
    }
    part2 = init_byteArray(original->size - split);
    if (part2 == NULL){
        destroy_byteArray(part1);
        return -1;
    }

    for(i = 0; i < original->size; i++){
        if(i < split) set_byteArray_byte(part1, original->bytes[i], i);
        else set_byteArray_byte(part2, original->bytes[i], i-split);
    }

    return 0;
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

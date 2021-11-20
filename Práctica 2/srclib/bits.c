#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../includes/bits.h"

// Estructura que guarda una lista de bits.
struct _Bits {
    int size; // Número de bits.
    char* bits; // Valores de los bits. La posición 0 corresponde al bit más significativo.
};

Bits* init_bits(int size){
    Bits* bits = NULL;
    int i;

    if(size <= 0) return NULL;
    if(size > 64){
        printf("Warning: Tamaños superiores a 64 bits pueden tener problemas al convertirse a enteros.\n");
    }

    bits = (Bits*) malloc (sizeof(Bits));
    if(bits == NULL) return NULL;

    bits->size = size;
    bits->bits = NULL;
    bits->bits = (char*) malloc (size*sizeof(char));
    if(bits->bits == NULL){
        free(bits);
        return NULL;
    }

    for(i = 0; i < size; i++){
        bits->bits[i] = ASCII_0;
    }

    return bits;
}

void destroy_bits(Bits* bits){
    if(bits != NULL){
        if(bits->bits != NULL) free(bits->bits);
        free(bits);
    }
    return;
}

Bits* copy_bits(Bits* bits){
    Bits* copied = NULL;
    int i;

    if(bits == NULL) return NULL;

    copied = init_bits(bits->size);
    if(copied == NULL) return NULL;

    for(i = 0; i < bits->size; i++){
        copied->bits[i] = bits->bits[i];
    }

    return copied;
}

int check_bits(Bits* bits){
    int i;

    // Control de errores
    if (bits == NULL) return -1;

    // Iteramos por los bits
    for (i=0; i<bits->size; i++) {
        // Comprobamos si el bit es correcto {0,1}
        if (bits->bits[i] != '0' && bits->bits[i] != '1') {
            return -1;
        }
    }

    return 0;
}

int set_byte_to_value(Bits* bits, int byte, unsigned char value){
    int i, j;

    if(bits == NULL || byte < 0) return -1;

    //No se puede escribir en ese byte (Está fuera del índice o está incompleto).
    if(bits->size/BYTE_SIZE <= byte) return -1;

    // Comprobamos si el byte está en notación binaria
    if (check_bits(bits) == -1) return -1;

    // Transformamos de decimal a binario
    for (i = 1<<(BYTE_SIZE-1), j = 0; i > 0; i=i/2, j++) {
        if (value >= i) {
            bits->bits[j+BYTE_SIZE*byte] = '1';
            value -= i;
        } else {
            bits->bits[j+BYTE_SIZE*byte] = '0';
        }
    }

    return 0;
}

unsigned char get_byte_value(Bits* bits, int byte){
    int i, j;
    unsigned char value = 0;

    // Control de errores
    if (bits == NULL) return 0;

    // Comprobamos si el byte está en notación binaria
    if (check_bits(bits) == -1) return 0;

    // Transformamos de binario a decimal
    for (i = 1<<(BYTE_SIZE-1), j = 0; i > 0; i=i/2, j++) {
        if (bits->bits[j] == '1') {
            value += i;
        }
    }

    return value;
}

int split_bits(Bits* original, int split, Bits** part1, Bits** part2){
    int i;

    // Control de errores. 
    if(!original || split < 1) return -1;
    if(original->size <= split) return -1;

    // Inicializamos la memoria de los sub arrays.
    *part1 = init_bits(split);
    if (*part1 == NULL){
        return -1;
    }
    *part2 = init_bits(original->size - split);
    if (*part2 == NULL){
        destroy_bits(*part1);
        return -1;
    }

    for(i = 0; i < original->size; i++){
        if(i < split) (*part1)->bits[i] = original->bits[i];
        else (*part2)->bits[i-split] = original->bits[i];
    }

    return 0;    
}

void print_bits(Bits* bits){
    if(!bits) return;

    printf("Tamaño: %d\n%.*s\n", bits->size, bits->size, bits->bits);
}
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
        if (bits->bits[i] != ASCII_0 && bits->bits[i] != ASCII_1) {
            return -1;
        }
    }

    return 0;
}

// Setters -----------
int set_bit(Bits* bits, int index, unsigned char value) {
    if (!bits) return -1;

    bits->bits[index] = value;

    return 0;
}

int set_bits_to_zero(Bits* bits) {
    int i;

    // Control de errores.
    if (bits == NULL || bits->bits == NULL) return -1;

    for (i = 0; i < bits->size; i++) {
        // Asignamos a 0 todos los bits
        bits->bits[i] = ASCII_0;
    }

    return 0;
}

int set_bits_decimal_value(Bits* bits, unsigned long long value){
    int j;
    unsigned long long i;

    if(bits == NULL || bits->size > 64) return -1;

    // Comprobamos si los bits está en notación binaria
    if (check_bits(bits) == -1) return -1;

    // Transformamos de decimal a binario
    for (i = 1ULL<<(bits->size-1), j = 0; i > 0; i=i/2, j++) {
        if (value >= i) {
            bits->bits[j] = '1';
            value -= i;
        } else {
            bits->bits[j] = '0';
        }
    }

    return 0;
}

int set_hex_to_value(Bits* bits, int hex, unsigned char value){
    int i, j, hex_size;

    if(bits == NULL || hex < 0 || value > 15) return -1;

    // Valores hexadecimales
    hex_size = 4;

    // No se puede escribir en ese hex (Está fuera del índice o está incompleto).
    if(bits->size/hex_size <= hex) return -1;

    // Comprobamos si los bits está en notación binaria.
    if (check_bits(bits) == -1) return -1;

    // Transformamos de decimal a binario.
    for (i = 1<<(hex_size-1), j = 0; i > 0; i=i/2, j++) {
        if (value >= i) {
            bits->bits[j+hex_size*hex] = '1';
            value -= i;
        } else {
            bits->bits[j+hex_size*hex] = '0';
        }
    }

    return 0;
}

int set_byte_to_value(Bits* bits, int byte, unsigned char value){
    int i, j;

    if(bits == NULL || byte < 0) return -1;

    // No se puede escribir en ese byte (Está fuera del índice o está incompleto).
    if(bits->size/BYTE_SIZE <= byte) return -1;

    // Comprobamos si los bits está en notación binaria.
    if (check_bits(bits) == -1) return -1;

    // Transformamos de decimal a binario.
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

int set_byte_desKey_value(Bits* bits, int byte, unsigned char value) {
    int i, j, counter = 0;

    // Control de errores.
    if (bits == NULL || value >= 128 || BYTE_SIZE != 8) return -1;

    // Comprobamos si los bits está en notación binaria.
    if (check_bits(bits) == -1) return -1;

    // Iteramos por los bits del byte
    for (i = 64, j = 0; i > 0; i=i/2, j++) {
        if (value >= i) {
            bits->bits[j + BYTE_SIZE*byte] = '1';
            value -= i;

            // Para el bit de paridad.
            counter++;
        } else {
            bits->bits[j + BYTE_SIZE*byte] = '0';
        }
    }

    // Asignamos el bit de paridad impar.
    if (counter % 2 == 0) {
        bits->bits[BYTE_SIZE*(byte+1)-1] = '1';
    } else {
        bits->bits[BYTE_SIZE*(byte+1)-1] = '0';
    }

    return 0;
}

int set_bits_desKey_value(Bits *bits) {
    int i;
    unsigned char random;

    // Control de errores.
    if (bits == NULL || bits->bits == NULL || bits->size != 64) return -1;

    // Utilizamos srand para tener números aleatorios en cada ejecución del programa
    srand(time(NULL));

    for (i = 0; i < bits->size/BYTE_SIZE; i++) {
        // Generamos un número positivo aleatorio de 7 bits
        random = rand()%128;

        // Asignamos el valor al byte
        if (set_byte_desKey_value(bits, i, random) == -1) return -1;
    }

    return 0;
}

// Getters -----------
unsigned char get_bit(Bits* bits, int index) {
    if (!bits) return 0;

    return bits->bits[index];
}

int get_bits_size(Bits* bits){
    if(!bits) return -1;

    return bits->size;
}

unsigned long long get_bits_decimal_value(Bits *bits) {
    int j;
    unsigned long long i, value = 0LL;

    // Control de errores
    if (bits == NULL || bits->size > 64) return 0LL;

    // Comprobamos si los bits está en notación binaria
    if (check_bits(bits) == -1) return 0LL;

    // Transformamos de binario a decimal
    for (i = 1ULL<<(bits->size-1), j = 0; i > 0; i=i/2, j++) {
        if (bits->bits[j] == '1') {
            value += i;
        }
    }

    return value;
}

unsigned char get_byte_value(Bits* bits, int byte){
    int i, j;
    unsigned char value = 0;

    // Control de errores
    if (bits == NULL) return 0;

    // Comprobamos si los bits está en notación binaria
    if (check_bits(bits) == -1) return 0;

    // Transformamos de binario a decimal
    for (i = 1<<(BYTE_SIZE-1), j = 0; i > 0; i=i/2, j++) {
        if (bits->bits[j+BYTE_SIZE*byte] == '1') {
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
    if(!part1 || !part2) return -1;

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

Bits* merge_bits(Bits* part1, Bits* part2){
    Bits* merged = NULL;
    int i;

    // Control de errores. 
    if(!part1 || !part2) return NULL;

    // Inicializamos la memoria de los sub arrays.
    merged = init_bits(part1->size + part2->size);
    if (merged == NULL){
        return NULL;
    }

    for(i = 0; i < merged->size; i++){
        if(i < part1->size) merged->bits[i] = part1->bits[i];
        else merged->bits[i] = part2->bits[i - part1->size];
    }

    return merged;
}

Bits *xor_bits(Bits *operand1, Bits *operand2) {
    Bits *result = NULL;
    int i;

    // Control de errores
    if (!operand1 || !operand2) return NULL;
    if (operand1->size != operand2->size) return NULL;

    // Inicializamos memoria para el resultado de la operación XOR
    result = init_bits(operand1->size);
    if (result == NULL) return NULL;

    // Calculamos el resultado de la operación XOR entre dos operandos
    for (i = 0; i < operand1->size; i++) {
        if (operand1->bits[i] != operand2->bits[i]) {
            result->bits[i] = ASCII_1;
        } else {
            result->bits[i] = ASCII_0;
        }
    }

    return result;
}

void print_bits(Bits* bits){
    if(!bits) return;

    printf("Tamaño: %dbits\n%.*s\n", bits->size, bits->size, bits->bits);
}

void print_bits_hex(Bits* bits){
    int i, value;

    if(!bits) return;
    if(bits->size%4!=0){
        printf("Tamaño: %d\nNo se puede mostrar en hexadecimal.\n", bits->size);
        return;
    }

    printf("Tamaño: %dbits\n", bits->size);

    for(i = 0; i < bits->size/4; i++){
        value = 0;
        if(bits->bits[i*4] == '1') value += 8;
        if(bits->bits[i*4+1] == '1') value += 4;
        if(bits->bits[i*4+2] == '1') value += 2;
        if(bits->bits[i*4+3] == '1') value += 1;

        if(value < 10) printf("%d", value);
        if(value == 10) printf("A");
        if(value == 11) printf("B");
        if(value == 12) printf("C");
        if(value == 13) printf("D");
        if(value == 14) printf("E");
        if(value == 15) printf("F");
    }

    printf("\n");
    
}

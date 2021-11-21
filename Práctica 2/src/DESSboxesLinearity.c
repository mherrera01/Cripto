#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../includes/DES_tables.h"
#include "../includes/bits.h"

/*
#define NUM_S_BOXES 8
#define ROWS_PER_SBOX 4
#define COLUMNS_PER_SBOX 16
*/

int getOuterValue(Bits* bits){
    int value = 0;

    if(!bits) return NULL;
    
    if(get_bit(bits, 0)) value += 2;
    if(get_bit(bits, 5)) value += 1;

    return value;
}

int getInnerValue(Bits* bits){
    int value = 0;

    if(!bits) return NULL;
    
    if(get_bit(bits, 1)) value += 8;
    if(get_bit(bits, 2)) value += 4;
    if(get_bit(bits, 3)) value += 2;
    if(get_bit(bits, 4)) value += 1;

    return value;
}

int main(){
    int i, j, k, linearTimes[NUM_S_BOXES];
    Bits *x=NULL, *y=NULL, *sx=NULL, *sy=NULL, *sz=NULL;

    x = init_bits(6);
    y = init_bits(6);

    sx = init_bits(4);
    sy = init_bits(4);
    sz = init_bits(4);

    for(i = 0; i < NUM_S_BOXES; i++) linearTimes[i] = 0;

    for(i = 0; i < 64; i++){
        set_bits_decimal_value(x, i);
        for(j = 0; j < 64; j++){
            set_bits_decimal_value(y, j);
            for(k = 0; k < NUM_S_BOXES; k++){
                set_bits_decimal_value(sx, S_BOXES[k][getOuterValue(x)][getInnerValue(x)]);
                set_bits_decimal_value(sy, S_BOXES[k][getOuterValue(y)][getInnerValue(y)]);
                
                //Comparar el Sbox de (x xor y) con el (Sbox de x) xor (Sbox de y).
                //Si son iguales se suman a linear times.
            }
        }
    }
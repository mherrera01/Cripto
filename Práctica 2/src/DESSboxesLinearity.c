#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../includes/DES_tables.h"
#include "../includes/bits.h"

int getOuterValue(Bits* bits){
    int value = 0;

    if(!bits) return 0;
    
    if(get_bit(bits, 0) == '1') value += 2;
    if(get_bit(bits, 5) == '1') value += 1;

    return value;
}

int getInnerValue(Bits* bits){
    int value = 0;

    if(!bits) return 0;
    
    if(get_bit(bits, 1) == '1') value += 8;
    if(get_bit(bits, 2) == '1') value += 4;
    if(get_bit(bits, 3) == '1') value += 2;
    if(get_bit(bits, 4) == '1') value += 1;

    return value;
}

int main(){
    int i, j, k, linearTimes[NUM_S_BOXES];
    Bits *x=NULL, *y=NULL, *z=NULL, *sx=NULL, *sy=NULL, *sz=NULL, *sxor=NULL;

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
                
                z = xor_bits(x, y);
                set_bits_decimal_value(sz, S_BOXES[k][getOuterValue(z)][getInnerValue(z)]);

                sxor = xor_bits(sx, sy);

                if(get_bits_decimal_value(sxor) == get_bits_decimal_value(sz)) linearTimes[k]++;

                destroy_bits(z);
                destroy_bits(sxor);
            }
        }
    }

    for(i = 0; i < NUM_S_BOXES; i++) printf("La %d S-box tiene un procentaje de no linealidad de: %lf%%\n", i, 100.0 - ((double)linearTimes[i]*100.0)/(64*64));

    destroy_bits(x);
    destroy_bits(y);

    destroy_bits(sx);
    destroy_bits(sy);
    destroy_bits(sz);

    return 0;
}
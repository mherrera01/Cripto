#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../includes/AES_tables.h"
#include "../includes/bits.h"

int get_hex_char_value(const char* hexChar){
    return strtol(hexChar, NULL, 16);
}

int getOuterValue(Bits* bits){
    int value = 0;

    if(!bits) return 0;

    if(get_bit(bits, 0) == '1') value += 8;
    if(get_bit(bits, 1) == '1') value += 4;
    if(get_bit(bits, 2) == '1') value += 2;
    if(get_bit(bits, 3) == '1') value += 1;
    
    return value;
}

int getInnerValue(Bits* bits){
    int value = 0;

    if(!bits) return 0;
    
    if(get_bit(bits, 4) == '1') value += 8;
    if(get_bit(bits, 5) == '1') value += 4;
    if(get_bit(bits, 6) == '1') value += 2;
    if(get_bit(bits, 7) == '1') value += 1;

    return value;
}

int main(){
    int i, j, linearTimes = 0;
    Bits *x=NULL, *y=NULL, *z=NULL, *sx=NULL, *sy=NULL, *sz=NULL, *sxor=NULL;

    x = init_bits(8);
    y = init_bits(8);

    sx = init_bits(8);
    sy = init_bits(8);
    sz = init_bits(8);

    for(i = 0; i < 256; i++){
        set_bits_decimal_value(x, i);
        for(j = 0; j < 256; j++){
            set_bits_decimal_value(y, j);

            set_bits_decimal_value(sx, get_hex_char_value(DIRECT_SBOX[getOuterValue(x)][getInnerValue(x)]));
            set_bits_decimal_value(sy, get_hex_char_value(DIRECT_SBOX[getOuterValue(y)][getInnerValue(y)]));
            
            z = xor_bits(x, y);
            set_bits_decimal_value(sz, get_hex_char_value(DIRECT_SBOX[getOuterValue(z)][getInnerValue(z)]));

            sxor = xor_bits(sx, sy);

            if(get_bits_decimal_value(sxor) == get_bits_decimal_value(sz)) linearTimes++;

            destroy_bits(z);
            destroy_bits(sxor);
        }
    }

    printf("La S-box tiene un procentaje de no linealidad de: %lf%%\n", 100.0 - ((double)linearTimes*100.0)/(256*256));

    destroy_bits(x);
    destroy_bits(y);

    destroy_bits(sx);
    destroy_bits(sy);
    destroy_bits(sz);

    return 0;
}
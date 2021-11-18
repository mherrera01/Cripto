#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../includes/byte.h"

char* init_byte(){
    char *byte = NULL;

    byte = (char*) malloc (8*sizeof(char));
    if (byte == NULL) return NULL;

    memset(byte, 0, 8);
    if (strlen(byte) != 8) {
        free(byte);
        return NULL;
    }

    return byte;
}

char* set_byte_to_value(int value, char* byte){
    int i, j;

    if(strlen(byte)!=8) return NULL;

    for(i = 128, j = 0; i > 0; i=i/2, j++){
        if(value >= i){
            byte[j] = '1';
            value -= i;
        } else{
            byte[j] = '0';
        }
    }

    return byte;
}

int get_byte_value(char* byte){
    int i, j, value = 0;

    if(strlen(byte)!=8) return -1;

    for(i = 128, j = 0; i > 0; i=i/2, j++){
        if(byte[j] == '1'){
            value += i;
        }
    }

    return value;
}

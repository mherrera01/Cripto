#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/alphabet.h"

#define MAX_KEY_SIZE 100

#define ENG_A 8.04
#define ENG_B 1.54
#define ENG_C 3.06
#define ENG_D 3.99
#define ENG_E 12.51
#define ENG_F 2.3
#define ENG_G 1.96
#define ENG_H 5.49
#define ENG_I 7.26
#define ENG_J 0.16
#define ENG_K 0.67
#define ENG_L 4.14
#define ENG_M 2.53
#define ENG_N 7.09
#define ENG_O 7.6
#define ENG_P 2.0
#define ENG_Q 0.11
#define ENG_R 6.12
#define ENG_S 6.54
#define ENG_T 9.25
#define ENG_U 2.71
#define ENG_V 0.99
#define ENG_W 1.92
#define ENG_X 0.19
#define ENG_Y 1.73
#define ENG_Z 0.19

#define ESP_A 11.96
#define ESP_B 0.92
#define ESP_C 2.92
#define ESP_D 6.87
#define ESP_E 16.78
#define ESP_F 0.52
#define ESP_G 0.73
#define ESP_H 0.89
#define ESP_I 4.15
#define ESP_J 0.3
#define ESP_K 0.0
#define ESP_L 8.37
#define ESP_M 2.12
#define ESP_N 7.01
#define ESP_O 8.69
#define ESP_P 2.77
#define ESP_Q 1.53
#define ESP_R 4.94
#define ESP_S 7.88
#define ESP_T 3.31
#define ESP_U 4.8
#define ESP_V 0.39
#define ESP_W 0.0
#define ESP_X 0.06
#define ESP_Y 1.54
#define ESP_Z 0.15

int main(int argc, char *argv[]){
    char ciphertext[1000000];
    char** savedStrings = NULL;
    int i, j, len, l=3;
    FILE* file;
    double IC[26];

    file = fopen("testfiles/test.txt","r");
    fread(

    len = strlen(ciphertext);

    for(i = 1; i < MAX_KEY_SIZE; i++){
        for(j = i; j < strlen; j++){

        }
    }
}

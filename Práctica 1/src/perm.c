#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/alphabet.h"
#include "../includes/matrix.h"
#include "../includes/padding.h"

int yPermutate(Matrix* original, Matrix* permutated, int* key_y){
    int i, j;

    if(!original || !permutated){
        return 1;
    }

    for(i = 0; i < get_matrix_size(original); i++){
        for(j = 0; j < get_matrix_size(original); j++){
            set_matrix_cell(permutated, j, key_y[i], get_matrix_cell(original, j, i));
        }
    }

    return 0;
}

int xPermutate(Matrix* original, Matrix* permutated, int* key_x){
    int i, j;

    if(!original || !permutated){
        return 1;
    }

    for(i = 0; i < get_matrix_size(original); i++){
        for(j = 0; j < get_matrix_size(original); j++){
            set_matrix_cell(permutated, key_x[i], j, get_matrix_cell(original, j, i));
        }
    }

    return 0;
}

Matrix* permutate_encrypt(Matrix* matrix, int* key_y, int* key_x){
    Matrix* halfPermutated = NULL;
    Matrix* permutated = NULL;

    if(!matrix || !key_x || !key_y){
        return NULL;
    }

    halfPermutated = init_matrix(get_matrix_size(matrix));
    if(halfPermutated == NULL) return NULL;

    if(yPermutate(matrix, halfPermutated, key_y)){
        destroy_matrix(halfPermutated);
        return NULL;
    }

    permutated = init_matrix(get_matrix_size(matrix));
    if(permutated == NULL) return NULL;

    if(xPermutate(halfPermutated, permutated, key_y)){
        destroy_matrix(halfPermutated);
        destroy_matrix(permutated);
        return NULL;
    }

    destroy_matrix(halfPermutated);
}

Matrix* load_text_to_matrix(char* block, int size){
    Matrix* matrix = NULL;
    int i, j;

    if(!block || size < 1){
        return 1;
    }

    matrix = init_matrix(size);
    if(matrix == NULL){
        return 1;
    }

    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            set_matrix_cell(matrix, j, i, get_letter_code(block[i*size+j]))
        }
    }
}

/*---------------------------- TODO ----------------------------*/
char* load_matrix_to_text(Matrix* matrix){
    char* block;
    int i, j;

    if(!matrix){
        return 1;
    }

    block = (char*) malloc ((matrix->size+1)*sizeof(char));

    if(matrix == NULL){
        return 1;
    }

    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            block[i*size+j] = get_letter(get_matrix_cell(matrix, j, i));
        }
    }
}

main(int argc, char* argv[]){
    char message[500];
    int key_y[4] = {1,2,3,4};
    int key_x[4] = {1,2,3,4};
    int size = 4;
}
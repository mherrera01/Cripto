#include <stdio.h>
#include <stdlib.h>
#include "../includes/matrix.h"

// Estructura que guarda la información sobre la matriz
struct _Matrix{
    int size; // Tamaño de la matriz
    int **matrix; // Elementos de la matriz
};

void destoy_matrix(Matrix* matrix) {
    int i;

    if (matrix == NULL) return;
    
    if (matrix->matrix != NULL) {
        for (i = 0; i < matrix->size; i++) {
            if (matrix->matrix[i] != NULL) {
                free(matrix->matrix[i]);
                matrix->matrix[i] = NULL;
            }
        }
        free(matrix->matrix);
        matrix->matrix = NULL;
    }

    free(matrix);
}

Matrix* init_matrix(int size) {
    Matrix *matrix = NULL;
    int i;

    // Comtrol de errores
    if (size <= 0) return NULL;

    // Alocamos memoria para la estructura
    matrix = (Matrix*) malloc (sizeof(Matrix));
    if (matrix == NULL) return NULL;

    // Asignamos el tamaño de la matriz
    matrix->size = size;
    matrix->matrix = (int**) malloc (size*sizeof(int*));
    if (matrix->matrix == NULL) {
        destoy_matrix(matrix);
        return NULL;
    }

    // Alocamos memoria para los elementos de la matriz
    for (i = 0; i < size; i++) {
        matrix->matrix[i] = (int*) malloc (size*sizeof(int));
        if (matrix->matrix[i] == NULL) {
            destoy_matrix(matrix);
            return NULL;
        }
    }

    return matrix;
}

//read matrix
//inverse matrix
    //suboperaciones para la inversa
//cifrar hill
    //suboperacion
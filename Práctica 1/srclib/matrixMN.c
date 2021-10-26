#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/matrixMN.h"

// Estructura que guarda la información sobre la matriz con M filas y N columnas
struct _MatrixMN {
    int sizeM; // Número de filas de la matriz
    int sizeN; // Número de columnas de la matriz
    int **matrix; // Elementos de la matriz
};

void destroy_matrixMN(MatrixMN* matrix) {
    int i;

    if (matrix == NULL) return;
    
    if (matrix->matrix != NULL) {
        for (i = 0; i < matrix->sizeM; i++) {
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

MatrixMN* init_matrixMN(int sizeM, int sizeN) {
    MatrixMN *matrix = NULL;
    int i;

    // Control de errores
    if (sizeM <= 0 || sizeN <= 0) return NULL;

    // Reservamos memoria para la estructura
    matrix = (MatrixMN*) malloc (sizeof(MatrixMN));
    if (matrix == NULL) return NULL;

    // Asignamos el tamaño de la matriz
    matrix->sizeM = sizeM;
    matrix->sizeN = sizeN;

    // Reservamos memoria para las filas de la matriz
    matrix->matrix = (int**) malloc (sizeM*sizeof(int*));
    if (matrix->matrix == NULL) {
        destroy_matrixMN(matrix);
        return NULL;
    }

    // Reservamos memoria para las columnas de la matriz
    for (i = 0; i < sizeM; i++) {
        matrix->matrix[i] = (int*) malloc (sizeN*sizeof(int));
        if (matrix->matrix[i] == NULL) {
            destroy_matrixMN(matrix);
            return NULL;
        }
    }

    return matrix;
}

// ---------------- Getters and Setters

int get_matrixMN_sizeM(MatrixMN* matrix) {
    if (matrix == NULL) return -1;
    return matrix->sizeM;
}

int get_matrixMN_sizeN(MatrixMN* matrix) {
    if (matrix == NULL) return -1;
    return matrix->sizeN;
}

int get_matrixMN_cell(MatrixMN* m, int x, int y) {
    // Control de errores
    if (m == NULL || m->sizeN <= x || m->sizeM <= y || x < 0 || y < 0) {
        return -1;
    }

    if (m->matrix[y] == NULL) return -1;
    return m->matrix[y][x];
}

int set_matrixMN_cell(MatrixMN* m, int x, int y, int value) {
    // Control de errores
    if (m == NULL || m->sizeN <= x || m->sizeM <= y || x < 0 || y < 0) {
        return -1;
    }

    if (m->matrix[y] == NULL) return -1;
    m->matrix[y][x] = value; // Asignamos el valor

    return 0;
}

void print_matrixMN(MatrixMN* matrix) {
    int i, j;

    // Comprobamos que la matriz se haya inicializado.
    if (matrix == NULL) return;

    // Mostramos el tamaño de la matriz.
    printf("Matrix of size: %dx%d\n", matrix->sizeM, matrix->sizeN);

    // Iteramos porr filas y columnas.
    for (i = 0; i < matrix->sizeM; i++) {
        printf("[");

        // En caso de que no se haya inicializado alguno de los valores, mostramos un error.
        if (matrix->matrix[i] == NULL) {
            printf("Error: Ha habido un error leyendo la fila %d de la matriz.\n", i);
            return;
        }

        for(j = 0; j < matrix->sizeN; j++){
            printf("%d\t", matrix->matrix[i][j]);
        }
        printf("]\n");
    }
}

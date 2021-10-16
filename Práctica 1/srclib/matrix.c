#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/matrix.h"
#include "../includes/euclides.h"

// Estructura que guarda la información sobre la matriz
struct _Matrix {
    int size; // Tamaño de la matriz
    int **matrix; // Elementos de la matriz
};

void destroy_matrix(Matrix* matrix) {
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

    // Control de errores.
    if (size <= 0) return NULL;

    // Reservamos memoria para la estructura.
    matrix = (Matrix*) malloc (sizeof(Matrix));
    if (matrix == NULL) return NULL;

    // Asignamos el tamaño de la matriz.
    matrix->size = size;
    matrix->matrix = (int**) malloc (size*sizeof(int*));
    if (matrix->matrix == NULL) {
        destroy_matrix(matrix);
        return NULL;
    }

    // Reservamos memoria para los elementos de la matriz.
    for (i = 0; i < size; i++) {
        matrix->matrix[i] = (int*) malloc (size*sizeof(int));
        if (matrix->matrix[i] == NULL) {
            destroy_matrix(matrix);
            return NULL;
        }
    }

    return matrix;
}

Matrix* read_matrix(FILE *file, int size, int m) {
    char *value = NULL, *convertToLong, readLine[80];
    Matrix* matrix = NULL;
    int i, j, valueInt = 0;

    if (file == NULL) return NULL;

    // Inicializamos la matriz con el tamaño indicado
    matrix = init_matrix(size);
    if (matrix == NULL) return NULL;

    // Leemos el archivo con la matriz de transformación
    for (i = 0; i < size; i++) {
        // Inicializamos la línea a leer del archivo
        memset(readLine, 0, sizeof(readLine));

        // Comprobamos si ha ocurrido un error o llegamos a EOF
        if (fgets(readLine, sizeof(readLine), file) == NULL) {
            destroy_matrix(matrix);
            return NULL;
        }
        readLine[strcspn(readLine, "\n")] = '\0';

        // Comprobamos si la línea del archivo es demasiado larga para leerla
        if (strlen(readLine) == sizeof(readLine)-1) {
            printf("Warning: La línea %d del archivo con la matriz es demasiado larga. Pueden surgir errores.\n", i+1);
        }

        // Obtenemos el primer elemento de la primera columna
        value = strtok(readLine, "|");
        if (value == NULL && size != 1) {
            destroy_matrix(matrix);
            return NULL;
        }

        // Convertimos el valor de la primera columna a Int
        valueInt = (int) strtol(value, &convertToLong, 10);

        // Comprobamos si no se ha convertido ningún caracter
        if (value == convertToLong) {
            destroy_matrix(matrix);
            return NULL;
        }

        // Obtenemos el valor equivalente a valueInt en el espacio Zm.
        valueInt %= m;
        if (valueInt < 0) {
            valueInt += m;
        }
        matrix->matrix[i][0] = valueInt; // Asignamos el elemento a la matriz

        // Obtenemos el resto de las columnas de la matriz
        for (j = 1; j < size; j++) {
            value = strtok(NULL, "|");

            // Error si no hay suficientes columnas 
            if (value == NULL) {
                destroy_matrix(matrix);
                return NULL;
            }

            // Convertimos el valor de la columna a Int
            valueInt = (int) strtol(value, &convertToLong, 10);

            // Comprobamos si no se ha convertido ningún caracter
            if (value == convertToLong) {
                destroy_matrix(matrix);
                return NULL;
            }

            // Obtenemos el valor equivalente a valueInt en el espacio Zm.
            valueInt %= m;
            if (valueInt < 0) {
                valueInt += m;
            }
            matrix->matrix[i][j] = valueInt; // Asignamos el elemento a la matriz
        }
    }

    return matrix;
}

int get_matrix_size(Matrix* matrix) {
    if (matrix == NULL) return -1;
    return matrix->size;
}

void print_matrix(Matrix* matrix) {
    int i, j;

    // Comprobamos que la matriz se haya inicializado.
    if(matrix == NULL) return;

    // Mostramos el tamaño de la matriz.
    printf("Matrix of size: %d\n", matrix->size);

    // Iteramos porr filas y columnas.
    for (i = 0; i < matrix->size; i++) {
        printf("[");

        // En caso de que no se haya inicializado alguno de los valores, mostramos un error.
        if (matrix->matrix[i] == NULL) {
            printf("Error: Ha habido un error leyendo la fila %d de la matriz.\n", i);
            return;
        }

        for(j = 0; j < matrix->size; j++){
            printf("%d\t", matrix->matrix[i][j]);
        }
        printf("]\n");
    }
}

Matrix* calculate_cofactor(Matrix* matrix, int y, int x) {
    Matrix* reduced = NULL;
    int i, j;

    // Control de errores
    if (matrix == NULL) {
        return NULL;
    }

    if (matrix->size <= x || matrix->size <= y) {
        return NULL;
    }

    // Inicializamos la matriz reducida
    reduced = init_matrix(matrix->size-1);
    if (reduced == NULL) return NULL;

    // Quitamos la fila y columna del elemento en la posición x,y
    for (i = 0; i < matrix->size-1; i++) {
        for (j = 0; j < matrix->size-1; j++) {
            if ((i >= y) && (j >= x)) reduced->matrix[i][j] = matrix->matrix[i+1][j+1];
            else if ((j >= x)) reduced->matrix[i][j] = matrix->matrix[i][j+1];
            else if ((i >= y)) reduced->matrix[i][j] = matrix->matrix[i+1][j];
            else reduced->matrix[i][j] = matrix->matrix[i][j];
        }
    }

    return reduced;
}

int calculate_determinant(Matrix* matrix) {
    int i, sign = 1, res = 0;
    Matrix *cofactor = NULL;

    if (matrix == NULL) return 0;

    // Matriz con un solo elemento
    if (matrix->size == 1) {
        return matrix->matrix[0][0];
    }

    for (i = 0; i < matrix->size; i++) {
        // Obtenemos el cofactor del elemento 0,i
        cofactor = calculate_cofactor(matrix, 0, i);
        if (cofactor == NULL) return 0;

        // Calculamos el resultado del determinante
        res += sign * matrix->matrix[0][i] * calculate_determinant(cofactor);

        // Liberamos la memoria de la matriz cofactor
        destroy_matrix(cofactor);
 
        // Alternamos el signo entre positivo y negativo
        sign = -sign;
    }

    return res;
}

Matrix* calculate_adjoint(Matrix* matrix) {
    Matrix *adjoint = NULL, *cofactor = NULL;
    int i, j;

    // Comprueba que la matriz exista
    if (matrix == NULL) return NULL;

    // Inicializa la matriz adjunta
    adjoint = init_matrix(matrix->size);
    if (adjoint == NULL) return NULL;

    // Obtiene el valor de cada casilla con el determinante de la matriz de cofactores para esa casilla
    for (i = 0; i < matrix->size; i++) {
        for (j = 0; j < matrix->size; j++) {
            // Obtenemos el cofactor del elemento i,j
            cofactor = calculate_cofactor(matrix, i, j);
            if (cofactor == NULL) {
                destroy_matrix(adjoint);
                return NULL;
            }

            // Calculamos el determinante de la matriz cofactor con su correspondiente signo
            if ((i+j)%2==1) adjoint->matrix[i][j] = -1*calculate_determinant(cofactor);
            else adjoint->matrix[i][j] = calculate_determinant(cofactor);

            // Liberamos la memoria de la matriz cofactor
            destroy_matrix(cofactor);
        }
    }

    // Devuelve la matriz adjunta
    return adjoint;
}

Matrix *calculate_transposed(Matrix *matrix) {
    int i, j;
    Matrix *transposed = NULL;

    // Control de errores
    if (matrix == NULL) return NULL;

    // Inicializamos la matriz transpuesta
    transposed = init_matrix(matrix->size);
    if (transposed == NULL) return NULL;

    // Cambiamos las columnas por las filas de la matriz
    for (i = 0; i < matrix->size; i++) {
        for (j = 0; j < matrix->size; j++) {
            transposed->matrix[j][i] = matrix->matrix[i][j];
        }
    }

    return transposed;
}

Matrix *calculate_inverse(Matrix *matrix, int m) {
    int i, j, det, detInverse, aux;
    Matrix *inverse = NULL, *adjoint = NULL, *transposedAdj = NULL;

    // Control de errores
    if (matrix == NULL || m <= 0) return NULL;
    
    // Inicializamos la matriz inversa
    inverse = init_matrix(matrix->size);
    if (inverse == NULL) return NULL;

    // Obtenemos el determinante de la matriz
    det = calculate_determinant(matrix) % m;

    // Si la matriz no tiene determinante, no tiene inversa
    if (det == 0) {
        destroy_matrix(inverse);
        return NULL;
    
    // Comprobamos si el determinante es negativo
    } else if (det < 0) {
        det += m;
    }

    // Obtenemos el inverso del determinante
    detInverse = get_modular_intverse(m, det);
    if (detInverse == 0) {
        destroy_matrix(inverse);
        return NULL;
    }

    // Obtenemos la matriz adjunta
    adjoint = calculate_adjoint(matrix);
    if (adjoint == NULL) {
        destroy_matrix(inverse);
        return NULL;
    }

    // Trasponemos la matriz adjunta
    transposedAdj = calculate_transposed(adjoint);
    if (transposedAdj == NULL) {
        destroy_matrix(adjoint);
        destroy_matrix(inverse);
        return NULL;
    }

    // Multiplicamos el inverso del determinante por la matriz adjunta
    for (i = 0; i < inverse->size; i++) {
        for (j = 0; j < inverse->size; j++) {
            aux = (detInverse * transposedAdj->matrix[i][j]) % m;
            if (aux < 0) {
                // Convertimos a positivo el elemento de la matriz inversa
                aux += m;
            }
            inverse->matrix[i][j] = aux;
        }
    }

    destroy_matrix(transposedAdj);
    destroy_matrix(adjoint);
    return inverse;
}

int* process_block(int* block, Matrix* key, int m) {
    int i, j, *ret = NULL;

    // Control de errores de los inputs.
    if (block == NULL || key == NULL || m <= 0) return NULL;

    // Creamos un array para devolver el resultado cifrado.
    ret = (int*) calloc (key->size, sizeof(int));
    if (ret == NULL) return NULL;

    // Calculamos el valor de cada caracter del bloque usando el cifrado de Hill.
    for (i=0; i<key->size; i++) {
        for (j=0; j<key->size; j++) {
            ret[i] += block[j] * key->matrix[j][i];
        }
        ret[i] = ret[i] % m;
    }

    return ret;
}

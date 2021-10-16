#ifndef MATRIX_H
#define MATRIX_H

typedef struct _Matrix Matrix;

// Libera la memoria de una matriz
void destroy_matrix(Matrix* matrix);

/**
 * Devuelve una matriz inicializada por el archivo y tamaño dados.
 * En caso de que el tamaño sea mayor que la matriz en el archivo o
 * ésta no sea cuadrada devuelve NULL. Mientras que si el tamaño es
 * menor, se leerán solo los elementos especificados.
 * 
 * Transforma los elementos de la matriz leídos a módulo m automáticamente.
 * 
 * Formato de matriz requerido en el archivo:
 * 1|4|2
 * 5|1|3
 * 1|8|2
 */
Matrix *read_matrix(FILE *file, int size, int m);

// Devuelve el tamaño de una matriz o -1 en caso de error
int get_matrix_size(Matrix* matrix);

// Imprime en pantalla una matriz
void print_matrix(Matrix* matrix);

// Calcula la inversa de una matriz dado un módulo
Matrix *calculate_inverse(Matrix *matrix, int m);

int* process_block(int* block, Matrix* key, int m);

#endif
#ifndef MATRIXMN_H
#define MATRIXMN_H

typedef struct _MatrixMN MatrixMN;

// Libera la memoria de una matriz MxN
void destroy_matrixMN(MatrixMN* matrix);

// Inicializa una matriz con M filas y N columnas
MatrixMN* init_matrixMN(int sizeM, int sizeN);

// Devuelve el número de filas de una matriz MxN o -1 en caso de error
int get_matrixMN_sizeM(MatrixMN* matrix);

// Devuelve el número de columnas de una matriz MxN o -1 en caso de error
int get_matrixMN_sizeN(MatrixMN* matrix);

// Devuelve el elemento en la posición x (columna), y (fila) de una matriz MxN o -1 en caso de error 
int get_matrixMN_cell(MatrixMN* m, int x, int y);

/**
 * Asigna el valor del elemento en la posición x (columna), y (fila) de una matriz MxN.
 * 
 * Devuelve 0 si se ha asignado correctamente el valor o -1 en caso de error
 */
int set_matrixMN_cell(MatrixMN* m, int x, int y, int value);

// Imprime en pantalla una matriz MxN
void print_matrixMN(MatrixMN* matrix);

#endif
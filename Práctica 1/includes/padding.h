#ifndef PADDING_H
#define PADDING_H

/**
 * Recibe una string y un tamaño de bloque y modifica la string para que tenga
 * padding para ese tamaño de bloque.
 * 
 * Devuelve -1 en caso de error y 0 en caso contrario
 */
int pad(char* string, int blockSize);

#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../includes/alphabet.h"
#include "../includes/padding.h"

int pad(char* string, int blockSize){
    int len, neededPad, i;

    // Control de errores.
    if(string == NULL || blockSize <= 0) return 1;

    // Calculamos cuantos caracteres de padding necesita.
    len = strlen(string);
    neededPad = len % blockSize;

    // Aumentamos el tamño de la string para que el padding entre.
    string = (char*) malloc(len+neededPad+1*sizeof(char));

    // Revisamos que el cambio de tamaño haya funcionado.
    if(string == NULL) return 1;

    // Añadimos padding con la primera letra del abecedario.
    for(i = len; i < len+neededPad; i++){
        string[i] = get_letter(0);
    }
    //Ponemos el caracter definal de string al final. (Esto no cuenta para el tamaño del padding).
    string[len+neededPad] = '\0';

    return 0;
}

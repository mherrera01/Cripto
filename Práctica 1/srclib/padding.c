#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../includes/alphabet.h"
#include "../includes/padding.h"

int pad(char* string, int blockSize) {
    int len, neededPad, i;

    // Control de errores.
    if (string == NULL || blockSize <= 0) return -1;

    // Calculamos cuantos caracteres de padding necesita.
    len = strlen(string);
    neededPad = blockSize - len;

    // Añadimos padding con la primera letra del abecedario.
    for (i = len; i < len+neededPad; i++) {
        string[i] = get_letter(0);
    }

    // Ponemos el caracter null terminated de string al final. (Esto no cuenta para el tamaño del padding).
    string[len+neededPad] = '\0';

    return 0;
}

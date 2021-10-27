#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../includes/coincidencia.h"
#include "../includes/alphabet.h"

// Tabla de frecuencia de las letras del alfabeto en inglés.
const double ENG[COINCIDENCE_ALPHABET_SIZE] = {8.04, 1.54, 3.06, 3.99, 12.51, 2.3, 1.96, 5.49, 7.26, 0.16, 0.67, 4.14, 2.53, 7.09, 7.6, 2.0, 0.11, 6.12, 6.54, 9.25, 2.71, 0.99, 1.92, 0.19, 1.73, 0.19};

// Tabla de frecuencia de las letras del alfabeto en castellano.
const double ESP[COINCIDENCE_ALPHABET_SIZE] = {11.96, 0.92, 2.92, 6.87, 16.78, 0.52, 0.73, 0.89, 4.15, 0.3, 0.0, 8.37, 2.12, 7.01, 8.69, 2.77, 1.53, 4.94, 7.88, 3.31, 4.8, 0.39, 0.0, 0.06, 1.54, 0.15};

void free_substrings(char **substrings, int keyLength) {
    int i;

    // Control de errores
    if (substrings == NULL || keyLength <= 0) return;

    // Liberamos memoria del array de char
    for (i = 0; i < keyLength; i++) {
        if (substrings[i] != NULL) {
            free(substrings[i]);
            substrings[i] = NULL;
        }
    }

    free(substrings);
}

char** divide_strings(char* string, int keyLength) {
    int i;
    char **substrings = NULL;

    // Control de errores
    if (string == NULL || keyLength <= 0) {
        return NULL;
    }

    // Reservamos memoria para dividir la cadena de texto
    substrings = (char**) malloc (keyLength * sizeof(char*));
    if (substrings == NULL) {
        free_substrings(substrings, keyLength);
        return NULL;
    }

    for (i = 0; i < keyLength; i++) {
        // Dividimos la cadena de texto por el tamaño de la clave
        substrings[i] = (char *) calloc ((strlen(string)/keyLength)+2, sizeof(char));
        if (substrings[i] == NULL) {
            free_substrings(substrings, keyLength);
            return NULL;
        }
    }

    // Guardamos la cadena de texto dividida
    for (i = 0; i < strlen(string); i++) {
        substrings[i%keyLength][i/keyLength] = string[i];
    }

    return substrings;
}

int calculate_key(char *string, int *frequency) {
    int i, j, key;
    double frequencyDouble = 0.0, maxFrequency = 0.0;

    // Control de errores
    if (string == NULL || frequency == NULL) return -1;

    // Obtenemos la frecuencia de cada letra del alfabeto en la cadena de texto
    for (i = 0; i < strlen(string); i++) {
        for (j = 0; j < COINCIDENCE_ALPHABET_SIZE; j++) {
            if (string[i] == get_letter(j)) {
                frequency[j]++;
                break;
            }
        }
    }

    // Obtenemos la letra con mayor frecuencia
    for (i = 0; i < COINCIDENCE_ALPHABET_SIZE; i++) {
        frequencyDouble = (double) frequency[i]/strlen(string);

        if (maxFrequency < frequencyDouble){
            maxFrequency = frequencyDouble;
            j = i;
        }
    }

    // La letra con mayor frecuencia en el inglés y en el castellano es la E
    key = get_letter_code('E');
    if (key == -1) return -1;

    key = j - key;
    if (key < 0) key += COINCIDENCE_ALPHABET_SIZE;

    return key;
}

char get_caesar_key(char* string) {
    int key, *frequency = NULL;

    // Control de errores
    if (string == NULL) return '_';

    // Reservamos memoria para la tabla de frecuencia de la cadena de texto
    frequency = (int*) calloc (COINCIDENCE_ALPHABET_SIZE, sizeof(int));
    if (frequency == NULL) return '_';

    // Calculamos la clave
    key = calculate_key(string, frequency);

    // Liberamos memoria
    free(frequency);

    return get_letter(key);
}

double check_key_length_IC(char* string, int keyLength, int language) {
    int i, key, cipherIndex;
    int* frequency = NULL;
    double frequencyDouble = 0.0, deviation = 0.0;

    // Control de errores
    if (string == NULL || keyLength <= 0) return -1;
    if (language != 0 && language != 1) return -1;

    // Reservamos memoria para la tabla de frecuencia de la cadena de texto
    frequency = (int*) calloc (COINCIDENCE_ALPHABET_SIZE, sizeof(int));
    if (frequency == NULL) return -1;

    // Calculamos la clave
    key = calculate_key(string, frequency);
    // Error calculando la clave
    if (key == -1) {
        free(frequency);
        return 0.0;
    }

    // Obtenemos la desviación total de cada letra
    for (i = 0; i < COINCIDENCE_ALPHABET_SIZE; i++) {
        // Calculamos la frecuencia de cada letra
        frequencyDouble = (double) frequency[i]/strlen(string);

        cipherIndex = (i-key >= 0 ? i-key : i-key+COINCIDENCE_ALPHABET_SIZE);

        // Calculamos el sumatorio de la probabilidad de que la letra xi del inglés coincida con la letra yi del mensaje cifrado.
        // Al ser x(A) la probabilidad de un caracter aleatorio de la cadena sea A en inglés e y(A) la probabilidad de que lo sea en el mensaje.
        deviation += frequencyDouble*((language ? ESP[cipherIndex] : ENG[cipherIndex])/100.0);
    }

    // Liberamos memoria
    free(frequency);

    return deviation;
}

#ifndef COINCIDENCIA_H
#define COINCIDENCIA_H

#define COINCIDENCE_ALPHABET_SIZE 26

void free_substrings(char **substrings, int keyLength);
char **divide_strings(char* string, int keyLength);
char get_caesar_key(char* string);
double check_key_length(char* string, int keyLength);

#endif
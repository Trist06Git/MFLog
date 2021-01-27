
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

extern int errno;

function* get_fdef(vector* defs, char* name) {
    for (int i = 0; i < size(defs); i++) {
        function* f = at(defs, i);
        if (strcmp(name, f->name) == 0) {
            return f;
        }
    }
    return NULL;
}

int digits(int num) {
    int res = 0;
    do {res++;} while (num/=10);
    return res;
}

int digits_neg(int num) {
    int res = num >= 0 ? 0 : 1;
    do {res++;} while (num/=10);
    return res;    
}

char* unique_name_incr(int* n) {
    char* new_name = malloc(sizeof(char)*digits(*n)+1);
    sprintf(new_name, "%s%i", unique_prefix, *n);
    (*n)++;
    return new_name;
}

char* unique_name(int* n) {
    char* new_name = malloc(sizeof(char)*digits(*n)+1);
    sprintf(new_name, "%s%i", unique_prefix, *n);
    return new_name;
}

//Unfinished, not needed for now.
/*char* file_to_string(const char* filename_path) {
    if (filename_path == NULL) {
        printf("Error. Invalid filename. null pointer given.\n");
        return NULL;
    }
    FILE* file = fopen(filename_path, "r");
    if (file == NULL) {
        printf("Error. Could not open file %s:\n  %s\n", filename_path, strerror(errno));
        return NULL;
    }
    return NULL;
}*/

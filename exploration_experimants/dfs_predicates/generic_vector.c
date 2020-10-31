
#include "generic_vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void push_back(vector* vec, void* element) {
    if (vec->allocated == 0) {
        //printf("init malloc size will be : %i\n", vec->el_size);
        vec->store = malloc(vec->el_size);
        vec->allocated = 1;
    } else if (vec->allocated == vec->count) {
        //printf("next realloc will be : %i\n", vec->el_size*vec->allocated*2);
        vec->store = realloc(vec->store, vec->el_size*vec->allocated*2);
        if (vec->store == NULL) printf("Error! something went wrong with realloc\n");
        vec->allocated *= 2;
    }
    memcpy((char*)vec->store+(vec->count*vec->el_size), element, vec->el_size);
    vec->count++;
}

void* at(vector* vec, int i) {
    if (i < vec->count) {
        return (char*)vec->store+(i*vec->el_size);
    } else {
        return (void*)-1;
    }
}

int size(vector* vec) {
    return vec->count;
}

char* type(vector* vec) {
    return vec->type;
}

vector* new_vector(int init_size, int el_size) {
    vector* res = malloc(sizeof(vector));
    res->el_size = el_size;
    res->count = 0;
    if (init_size <= 0) {
        res->store = NULL;
        res->allocated = 0;
    } else {
        res->store = malloc(sizeof(res->el_size)*init_size);
        res->allocated = init_size;
    }
    /*printf("created vector ::\n");
    printf("init allocated : %i\n", res->allocated);
    printf("element size   : %i\n\n", res->el_size);*/
    return res;
}

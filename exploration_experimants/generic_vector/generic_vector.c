
#include "generic_vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void push_back(vector* vec, void* element) {
    if (vec->allocated == 0) {
        printf("init malloc\n");
        vec->store = malloc(sizeof(vec->el_size));
        vec->allocated = 1;
    } else if (vec->allocated == vec->count) {
        printf("expand realloc\n");
        vec->store = realloc(vec->store, sizeof(vec->el_size)*vec->allocated*2);
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
        res->store = malloc(sizeof(vector)*res->el_size);
        res->allocated = init_size;
    }
    return res;
}

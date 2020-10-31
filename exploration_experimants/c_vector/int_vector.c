
#include "int_vector.h"

#include <stdlib.h>
#include <stdio.h>

void push_back(int_vector* vec, int element) {
    if (vec->allocated == 0) {
        printf("init malloc\n");
        vec->store = malloc(sizeof(int));
        vec->allocated = 1;
    } else if (vec->allocated == vec->count) {
        printf("expand realloc\n");
        vec->store = realloc(vec->store, sizeof(int)*vec->allocated*2);
        vec->allocated *= 2;
    }
    vec->store[vec->count] = element;
    vec->count++;
    
}

int at(int_vector* vec, int i) {
    if (i < vec->count) {
        return vec->store[i];
    } else {
        return -1;
    }
}

int size(int_vector* vec) {
    return vec->count;
}

int_vector* new_int_vector(int init_size) {
    int_vector* res = malloc(sizeof(int_vector));
    res->count = 0;
    res->allocated = 0;
    res->store = init_size == 0 ? NULL : malloc(sizeof(int_vector)*init_size);
    return res;
}

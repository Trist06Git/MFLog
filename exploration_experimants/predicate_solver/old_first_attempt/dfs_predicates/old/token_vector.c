
#include "token_vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

s_token error = {.tag = ERROR, .name = ""};

void push_back(token_vector* vec, s_token element) {
    if (vec->allocated == 0) {
        vec->store = malloc(sizeof(s_token));
        vec->allocated = 1;
    } else if (vec->allocated == vec->count) {
        vec->store = realloc(vec->store, sizeof(s_token)*vec->allocated*2);
        vec->allocated *= 2;
    }

    memcpy(vec->store+vec->count, &element, sizeof(element));
    vec->count++;
    //vec->store[vec->count] = element;
    
}

s_token* at(token_vector* vec, int i) {
    if (i < vec->count) {
        return vec->store+i;
    } else {
        return &error;
    }
}

int size(token_vector* vec) {
    return vec->count;
}

token_vector* new_token_vector(int init_size) {
    token_vector* res = malloc(sizeof(token_vector));
    res->count = 0;
    res->allocated = 0;
    res->store = init_size == 0 ? NULL : malloc(sizeof(s_token)*init_size);
    return res;
}

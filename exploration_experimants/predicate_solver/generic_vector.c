
#include "generic_vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void push_back(vector* vec, void* element) {
    if (vec->allocated == 0) {
        vec->store = malloc(vec->el_size*2);//init with 2 to allow insert shuffle
        vec->allocated = 1;
    } else if (vec->allocated == vec->count) {
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

int remove_at(vector* vec, int i) {
    if (vec == NULL || i >= vec->count) return -1;
    for (int j = i+1; j < vec->count; j++) {
        memcpy((char*)vec->store + vec->el_size*(j-1), (char*)vec->store + vec->el_size*(j), vec->el_size);
    }
    vec->count--;

    return 0;
}

int insert_at(vector* vec, int i, void* element) {
    if (vec->allocated == 0) {
        vec->store = malloc(vec->el_size*2);//init with 2 to allow insert shuffle
        vec->allocated = 1;
    } else if (vec->allocated == vec->count) {
        vec->store = realloc(vec->store, vec->el_size*vec->allocated*2);
        if (vec->store == NULL) {
            printf("Error! something went wrong with realloc\n");
            return -1;
        }
        vec->allocated *= 2;
        //move accross
        for (int j = vec->count+1; j > i; j--) {
            memcpy((char*)vec->store + vec->el_size*(j), (char*)vec->store + vec->el_size*(j-1), vec->el_size);
        }
    } else {
        for (int j = vec->count+1; j > i; j--) {
            memcpy((char*)vec->store + vec->el_size*(j), (char*)vec->store + vec->el_size*(j-1), vec->el_size);
        }
    }
    memcpy((char*)vec->store + vec->el_size*i, element, vec->el_size);
    
    vec->count++;
    return 0;
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

void free_vector(vector* vec) {
    if (vec == NULL) return;
    if (vec->store != NULL) {
        free(vec->store);
    }
    free(vec);
}

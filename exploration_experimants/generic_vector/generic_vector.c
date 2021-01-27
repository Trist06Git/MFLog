
#include "generic_vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void push_back(vector* vec, void* element) {
    if (vec->allocated == 0) {
        vec->store = malloc(vec->el_size*2);//init with 2 to allow insert shuffle
        vec->allocated = 2;//should be 2??
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
        return (void*)-1;//should be null
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
        vec->allocated = 2;//should be 2??
    } else if (vec->allocated == vec->count) {
        vec->store = realloc(vec->store, vec->el_size*vec->allocated*2);
        if (vec->store == NULL) {
            printf("Error! something went wrong with realloc\n");
            return -1;
        }
        vec->allocated *= 2;
        //move accross
        for (int j = vec->count; j > i; j--) {
            memcpy((char*)vec->store + vec->el_size*(j), (char*)vec->store + vec->el_size*(j-1), vec->el_size);
        }
    } else {
        for (int j = vec->count; j > i; j--) {
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

bool contains(vector* vec, void* item) {
    for (int i = 0; i < size(vec); i++) {
        void* element = at(vec, i);
        bool res = true;
        for (int j = 0; j < vec->el_size; j++) {
            res &= ((char*)element)[j] == ((char*)item)[j];
        }
        if (res == true) return true;
    }
    return false;
}

bool contains_string(vector* vec, const char* str) {
    for (int i = 0; i < size(vec); i++) {
        char** element = at(vec, i);
        if (strcmp(*element, str) == 0) return true;
    }
    return false;
}

//the start of vec2 will be connected to the end of vec1
//vec1 will be modified in situ, and vec2 will not be freed
//WARN! no type checking...
void append_vector(vector* vec1, vector* vec2) {
    int target_size = vec1->allocated;
    while (target_size < size(vec1)+size(vec2)) {
        target_size *= 2;
    }
    vec1->store = realloc(vec1->store, vec1->el_size*target_size);
    if (vec1->store == NULL) printf("Error! something went wrong with realloc\n");
    vec1->allocated = target_size;
    for (int i = 0; i < size(vec2); i++) {//O(n) insertion
        push_back(vec1, at(vec2, i));
    }
}

//copies orig_vec into a newly allocated vector
vector* duplicate_vector(const vector* orig_vec) {
    vector* new_vec = malloc(sizeof(vector));
    new_vec->count     = orig_vec->count;
    new_vec->allocated = orig_vec->allocated;
    new_vec->el_size   = orig_vec->el_size;
    new_vec->store = malloc(orig_vec->el_size*orig_vec->allocated);
    memcpy(new_vec->store, orig_vec->store, orig_vec->el_size*orig_vec->count);
    return new_vec;
}

vector* new_vector(int init_size, int el_size) {
    vector* res = malloc(sizeof(vector));
    res->el_size = el_size;
    res->count = 0;
    if (init_size <= 0) {
        res->store = NULL;
        res->allocated = 0;
    } else {
        //if (init_size == 1) init_size++;//bad hack for insert shuffle
        res->store = malloc((res->el_size)*init_size);
        res->allocated = init_size;
    }
    return res;
}

void free_vector(vector* vec) {
    if (vec == NULL) return;
    if (vec->store != NULL) {
        free(vec->store);
    }
    free(vec);
}

//old
char* type(vector* vec) {
    return vec->type;
}

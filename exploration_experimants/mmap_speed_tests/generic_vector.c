
#include "generic_vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void vec_push_back(vector* vec, void* element) {
    if (vec->allocated == 0) {
        vec->store = malloc(vec->el_size*2);//init with 2 to allow insert shuffle
        vec->allocated = 2;
    } else if (vec->allocated == vec->count) {
        vec->store = realloc(vec->store, vec->el_size*vec->allocated*2);
        if (vec->store == NULL) printf("Error! something went wrong with realloc\n");
        vec->allocated *= 2;
    }
    memcpy((char*)vec->store+(vec->count*vec->el_size), element, vec->el_size);
    vec->count++;
}

void* vec_at(vector* vec, long int i) {
    if (i < vec->count) {
        return (char*)vec->store+(i*vec->el_size);
    } else {
        return NULL;//should be null
    }
}

int vec_remove_at(vector* vec, long int i) {
    if (vec == NULL || i >= vec->count) return -1;
    for (long int j = i+1; j < vec->count; j++) {
        memcpy((char*)vec->store + vec->el_size*(j-1), (char*)vec->store + vec->el_size*(j), vec->el_size);
    }
    vec->count--;

    return 0;
}

int vec_insert_at(vector* vec, long int i, void* element) {
    if (vec->allocated == 0) {
        vec->store = malloc(vec->el_size*2);//init with 2 to allow insert shuffle
        vec->allocated = 2;
    } else if (vec->allocated == vec->count) {
        vec->store = realloc(vec->store, vec->el_size*vec->allocated*2);
        if (vec->store == NULL) {
            printf("Error! something went wrong with realloc\n");
            return -1;
        }
        vec->allocated *= 2;
        //move accross
        for (long int j = vec->count; j > i; j--) {
            memcpy((char*)vec->store + vec->el_size*(j), (char*)vec->store + vec->el_size*(j-1), vec->el_size);
        }
    } else {
        for (long int j = vec->count; j > i; j--) {
            memcpy((char*)vec->store + vec->el_size*(j), (char*)vec->store + vec->el_size*(j-1), vec->el_size);
        }
    }
    memcpy((char*)vec->store + vec->el_size*i, element, vec->el_size);
    
    vec->count++;
    return 0;
}

long int vec_size(vector* vec) {
    return vec->count;
}

bool vec_contains(vector* vec, void* item) {
    for (int i = 0; i < vec_size(vec); i++) {
        void* el = vec_at(vec, i);
        int res = vec->comparator(el, item, vec->el_size);
        if (res == 0) return true;
    }
    return false;
}

//bool vec_contains(vector* vec, void* item) {
//    for (int i = 0; i < vec_size(vec); i++) {
//        void* element = vec_at(vec, i);
//        bool res = true;
//        for (int j = 0; j < vec->el_size; j++) {
//            res &= ((char*)element)[j] == ((char*)item)[j];
//        }
//        if (res == true) return true;
//    }
//    return false;
//}

bool vec_contains_string(vector* vec, const char* str) {
    for (long int i = 0; i < vec_size(vec); i++) {
        char** element = vec_at(vec, i);
        if (strcmp(*element, str) == 0) return true;
    }
    return false;
}

//the start of vec2 will be connected to the end of vec1
//vec1 will be modified in situ, and vec2 will not be freed
//WARN! no type checking...
void append_vector(vector* vec1, vector* vec2) {
    long int target_size = vec1->allocated;
    while (target_size < vec_size(vec1)+vec_size(vec2)) {
        target_size *= 2;
    }
    vec1->store = realloc(vec1->store, vec1->el_size*target_size);
    if (vec1->store == NULL) printf("Error! something went wrong with realloc\n");
    vec1->allocated = target_size;
    for (long int i = 0; i < vec_size(vec2); i++) {//O(n) insertion
        vec_push_back(vec1, vec_at(vec2, i));
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

vector* new_vector(long int init_size, int el_size) {
    vector* res = malloc(sizeof(vector));
    res->el_size = el_size;
    res->count = 0;
    res->comparator = vec_byte_compare;
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

void vec_set_comparator(vector* vec, int (*comp)(const void*, const void*, int)) {
    vec->comparator = comp;
}

int vec_byte_compare(const void* e1, const void* e2, int bytes) {
    const char* c_lhs = e1;
    const char* c_rhs = e2;
    for (int i = 0; i < bytes; i++) {
        //if (*(char*)e1 != *(char*)e2) return false;
        if (c_lhs[i] != c_rhs[i]) return 1;
    }
    return 0;
}

int vec_string_compare(const void* el1, const void* el2, int size) {
    //size not needed..
    const char* str1 = *(char**)el1;
    const char* str2 = *(char**)el2;
    return strcmp(str1, str2);
}

//old
char* vec_type(vector* vec) {
    return vec->type;
}

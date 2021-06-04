#ifndef GENERIC_VECTOR_H
#define GENERIC_VECTOR_H

#include <stdbool.h>

typedef struct vector {
    long int count;
    long int allocated;
    void* store;
    int el_size;
    int (*comparator)(const void*, const void*, int);
    char* type; //for optional safety and debugging
} vector;

void vec_push_back(vector*, void*);
void* vec_at(vector*, long int);
int vec_remove_at(vector*, long int);
int vec_insert_at(vector*, long int, void*);
long int vec_size(vector*);
bool vec_contains(vector*, void*);//should probably make some of these const
bool vec_contains_string(vector*, const char*);
void append_vector(vector*, vector*);
vector* duplicate_vector(const vector*);
vector* new_vector(long int, int);
void free_vector(vector*);
void vec_set_comparator(vector*, int (*)(const void*, const void*, int));
int vec_byte_compare(const void* e1, const void* e2, int bytes);
int vec_string_compare(const void* el1, const void* el2, int size);
char* vec_type(vector*);//old

#endif
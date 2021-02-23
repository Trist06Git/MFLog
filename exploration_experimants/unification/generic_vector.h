#ifndef GENERIC_VECTOR_H
#define GENERIC_VECTOR_H

#include <stdbool.h>

typedef struct vector {
    int count;
    int allocated;
    void* store;
    int el_size;
    char* type; //for optional safety and debugging
} vector;

void vec_push_back(vector*, void*);
void* vec_at(vector*, int);
int vec_remove_at(vector*, int);
int vec_insert_at(vector*, int, void*);
int vec_size(vector*);
bool vec_contains(vector*, void*);//should probably make some of these const
bool vec_contains_string(vector*, const char*);
void append_vector(vector*, vector*);
vector* duplicate_vector(const vector*);
vector* new_vector(int, int);
void free_vector(vector*);
char* vec_type(vector*);//old

#endif
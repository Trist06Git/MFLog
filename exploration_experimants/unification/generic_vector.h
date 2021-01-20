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

void push_back(vector*, void*);
void* at(vector*, int);
int remove_at(vector*, int);
int insert_at(vector*, int, void*);
int size(vector*);
bool contains(vector*, void*);//should probably make some of these const
bool contains_string(vector*, const char*);
void append_vector(vector*, vector*);
vector* new_vector(int, int);
void free_vector(vector*);
char* type(vector*);//old

#endif
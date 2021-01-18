#ifndef GENERIC_VECTOR_H
#define GENERIC_VECTOR_H

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
char* type(vector*);
vector* new_vector(int, int);
void free_vector(vector*);//...

#endif
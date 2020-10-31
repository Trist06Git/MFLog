#ifndef INT_VECTOR_H
#define INT_VECTOR_H

typedef struct int_vector {
    int count;
    int allocated;
    int* store;
} int_vector;

void push_back(int_vector*, int);
int at(int_vector*, int);
int size(int_vector*);
int_vector* new_int_vector(int);
void free_int_vector(int_vector*);//...

#endif
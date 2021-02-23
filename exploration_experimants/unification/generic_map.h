#ifndef GENERIC_MAP_H
#define GENERIC_MAP_H

#include <stdbool.h>

#include "generic_vector.h"

typedef struct pair {
    void* fst;
    void* snd;
} pair;

typedef struct map {
    vector* store;
    int el1_size;
    int el2_size;
    bool (*el1_comparator)(void*, void*, int);
    bool (*el2_comparator)(void*, void*, int);
} map;

void map_push_back(map*, void*, void*);
void map_add(map*, void*, void*);//alias
void* map_at(map*, void*);//alias
void* map_at_index(map*, int);
void* fst(map*, void*);
void* snd(map*, void*);
int map_remove(map*, void*);
bool map_contains_key(map*, void*);
bool map_contains_val(map*, void*);
int map_size(map*);
void set_fst_comparator(map*, bool (*)(void*, void*, int));
void set_snd_comparator(map*, bool (*)(void*, void*, int));
bool byte_compare(void* e1, void* e2, int bytes);
bool string_compare(void*, void*, int);
map* new_map(int, int);
void free_map(map*);

#endif
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
    bool (*el1_comparator)(const void*, const void*, int);
    bool (*el2_comparator)(const void*, const void*, int);
} map;

void map_push_back(const map*, const void*, const void*);
void map_add(const map*, const void*, const void*);//alias
void* map_at(map*, void*);//alias
void* map_at_index(map*, int);
void* fst(map*, void*);
void* snd(map*, void*);
int map_remove(map*, void*);
bool map_contains_key(const map*, const void*);
bool map_contains_val(const map*, const void*);
int map_size(const map*);
void set_fst_comparator(map*, bool (*)(const void*, const void*, int));
void set_snd_comparator(map*, bool (*)(const void*, const void*, int));
bool byte_compare(const void* e1, const void* e2, int bytes);
bool string_compare(const void*, const void*, int);
map* new_map(int, int);
void free_map(map*);

#endif
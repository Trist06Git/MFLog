#ifndef COW_MAP_H
#define COW_MAP_H

#include <stdint.h>

#include "generic_vector.h"

static int unique_num = 0;

typedef struct cow_map cow_map;

struct cow_map {
    vector* dirty;//pages, as ints
    uint8_t* parent;
    int max_size;
    int pages;
    int element_size;
};

cow_map* new_cow_map(int element_size);
cow_map* duplicate_cow_map(cow_map*);
bool cow_map_insert(cow_map*, void*, int addr);
void* cow_map_at(cow_map*, int addr);
int new_anon_file(int size);
char* unique_name(void);
void cow_map_info(cow_map*);
int digits(int num);

#endif
#ifndef MLOG_VECTOR_H
#define MLOG_VECTOR_H

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
//#include <maths.h>
#include <math.h>

static size_t init_array_size;
static size_t page_size;
static size_t aligned_array_size;
static bool inited = false;

//pos for positive
//neg for negative
typedef struct {
    size_t el_size;
    
    int count_pos;//will be used in negative comparison
    size_t alloced_pos;//needed for munmap
    size_t precalc_alloced_pos;//save on a division each insert
    uint8_t* store_pos;
    
    int count_neg;
    size_t alloced_neg;
    size_t precalc_alloced_neg;
    uint8_t* store_neg;
} mf_array;

void* mfa_pop_back(mf_array*);
void* mfa_pop_front(mf_array*);
bool mfa_push_back(mf_array*, void* element);
bool mfa_push_front(mf_array*, void* element);
bool mfa_remove_back(mf_array*);
bool mfa_remove_front(mf_array*);
bool mfa_set(mf_array*, int i, void* element);
void* mfa_at(mf_array*, int i);
bool mfa_remove_at(mf_array*, int i);
size_t mfa_card(mf_array*);
mf_array* mfa_duplicate(mf_array*);
bool mfa_compare(mf_array*, mf_array*);

mf_array* new_mfarray(size_t el_size);
void free_mfarray(mf_array*);
uint8_t* new_store(size_t);
void free_store(uint8_t* store, size_t alloced);
bool init_mfarrays(void);
size_t align_to_page(size_t target_size, size_t page_size);

#endif
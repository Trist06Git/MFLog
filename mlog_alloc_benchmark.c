
//set these with -D for gcc
//#define MREMAP_EXPANSION
//#define REALLOC_EXPANSION
//#define PRE_EXPANDED

#ifndef MREMAP_EXPANSION
#ifndef REALLOC_EXPANSION
#ifndef PRE_EXPANDED
#error "No #define for benchmake type. See mloc_alloc*.c for details"
#endif
#endif
#endif

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef MREMAP_EXPANSION
#define __USE_GNU
#endif
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <math.h>

#include "mlog_alloc_benchmark.h"

void free_array(mf_array* arr) {
    free_store(arr->store_pos, arr->alloced_pos);
    free(arr);
}

#ifdef PRE_EXPANDED
//NULL is failure
mf_array* new_array(size_t el_size) {
    if (!inited) init_arrays();
    mf_array* arr = malloc(sizeof(mf_array));
    size_t aligned = align_to_page(array_size, page_size);
    arr->store_pos = new_store(aligned);
    if (arr->store_pos == NULL) {
        printf("Error, new_store() pos failed.\n");
        free(arr);
        return NULL;
    }

    arr->el_size = el_size;
    arr->count_pos = 0;
    arr->alloced_pos = aligned;

    return arr;
}

//NULL is failure
uint8_t* new_store(size_t to_alloc) {
    uint8_t* store = mmap(
        NULL,                           //dont care where the mapping is
        to_alloc,                
        PROT_READ|PROT_WRITE|PROT_EXEC, //protections
        MAP_ANONYMOUS|MAP_PRIVATE,           
        -1,                             
        0                               //offset into mapped file, 0 for anon
    );
    if (store == MAP_FAILED) {
        perror("Error, mmap() failed");
        return NULL;
    } else {
        //printf("mapped to address: %p\n", store);
    }
    return store;
}

void free_store(uint8_t* store, size_t alloced) {
    int result = munmap(store, alloced);
    if (result) {
        perror("Error, munmap() failed");
    }
}

//NULL is failure
void* at(mf_array* arr, size_t i) {
    if (i < arr->count_pos) {
        return arr->store_pos + (i*arr->el_size);
    } else {
        return NULL;
    }
}

//false/0 is failure
bool set(mf_array* arr, size_t i, void* element) {
    if (i > arr->count_pos+1) {
        return false;
    } else if (arr->count_pos >= arr->alloced_pos) {
        //hit the 4g limit
        return false;
    } else {
        uint8_t* i_element = element;
        for (int j = 0; j < arr->el_size; j++) {//j bytes long
            arr->store_pos[(i*arr->el_size) + j] = i_element[j];
        }
        if (i == arr->count_pos) arr->count_pos++;//append
        return true;
    }
}
#elif defined MREMAP_EXPANSION
//NULL is failure
mf_array* new_array(size_t el_size) {
    if (!inited) init_arrays();
    mf_array* arr = malloc(sizeof(mf_array));
    arr->store_pos = new_store(page_size);
    if (arr->store_pos == NULL) {
        printf("Error, new_store() pos failed.\n");
        free(arr);
        return NULL;
    }

    arr->el_size = el_size;
    arr->count_pos = 0;
    arr->alloced_pos = page_size;

    return arr;
}

//NULL is failure
uint8_t* new_store(size_t to_alloc) {
    uint8_t* store = mmap(
        NULL,                           //dont care where the mapping is
        to_alloc,                
        PROT_READ|PROT_WRITE|PROT_EXEC, //protections
        MAP_ANONYMOUS|MAP_PRIVATE,           
        -1,                             
        0                               //offset into mapped file, 0 for anon
    );
    if (store == MAP_FAILED) {
        perror("Error, mmap() failed");
        return NULL;
    } else {
        //printf("mapped to address: %p\n", store);
    }
    return store;
}

void free_store(uint8_t* store, size_t alloced) {
    int result = munmap(store, alloced);
    if (result) {
        perror("Error, munmap() failed");
    }
}

//NULL is failure
void* at(mf_array* arr, size_t i) {
    if (i < arr->count_pos) {
        return arr->store_pos + (i*arr->el_size);
    } else {
        return NULL;
    }
}

//false/0 is failure
bool set(mf_array* arr, size_t i, void* element) {
    if (i > arr->count_pos+1) {
        return false;
    } else {
        if (arr->count_pos*arr->el_size >= arr->alloced_pos) {//expand
            arr->store_pos = mremap(
                arr->store_pos,          //old addr
                arr->alloced_pos,       //old size
                arr->alloced_pos*2,    //new size
                MREMAP_MAYMOVE
            );
            if (arr->store_pos == MAP_FAILED) {
                printf("Error. mremap() failed while tring to expand from %li to %li.\n", arr->alloced_pos, arr->alloced_pos*2);
                printf("Exiting now\n");
                perror("");
                exit(EXIT_FAILURE);
            }
            arr->alloced_pos *= 2;
        }
        uint8_t* i_element = element;
        for (int j = 0; j < arr->el_size; j++) {//j bytes long
            arr->store_pos[(i*arr->el_size) + j] = i_element[j];
        }
        if (i == arr->count_pos) arr->count_pos++;//append
        return true;
    }
}
#elif defined REALLOC_EXPANSION
//NULL is failure
mf_array* new_array(size_t el_size) {
    if (!inited) init_arrays();
    mf_array* arr = malloc(sizeof(mf_array));
    arr->store_pos = new_store(el_size);
    if (arr->store_pos == NULL) {
        printf("Error, new_store() pos failed.\n");
        free(arr);
        return NULL;
    }

    arr->el_size = el_size;
    arr->count_pos = 0;
    arr->alloced_pos = el_size;

    return arr;
}

//NULL is failure
uint8_t* new_store(size_t el_size) {
    uint8_t* store = malloc(el_size);
    if (store == NULL) {
        perror("Error, mmap() failed");
        return NULL;
    } else {
        //printf("mapped to address: %p\n", store);
    }
    return store;
}

void free_store(uint8_t* store, size_t alloced) {
    free(store);
}

//NULL is failure
void* at(mf_array* arr, size_t i) {
    if (i < arr->count_pos) {
        return arr->store_pos + (i*arr->el_size);
    } else {
        return NULL;
    }
}

//false/0 is failure
bool set(mf_array* arr, size_t i, void* element) {
    if (i > arr->count_pos+1) {
        return false;
    } else {
        if (arr->count_pos*arr->el_size >= arr->alloced_pos) {//expand
            arr->store_pos = realloc(arr->store_pos, arr->alloced_pos*2);
            if (arr->store_pos == NULL) {
                printf("Error. realloc() failed while tring to expand from %li to %li.\n", arr->alloced_pos, arr->alloced_pos*2);
                printf("Exiting now\n");
                perror("");
                exit(EXIT_FAILURE);
            }
            arr->alloced_pos *= 2;
        }
        uint8_t* i_element = element;
        for (int j = 0; j < arr->el_size; j++) {//j bytes long
            arr->store_pos[(i*arr->el_size) + j] = i_element[j];
        }
        if (i == arr->count_pos) arr->count_pos++;//append
        return true;
    }
}
#endif

int card(mf_array* arr) {
    return arr->count_pos;
}

bool init_arrays(void) {
    //manually fetched from getrlimit(), see user_vm_space_stats.c for details
    array_size = sqrt(18446744073709551615.0);
    page_size = getpagesize();
    aligned_array_size = align_to_page(array_size, page_size);
    inited = true;
}

//only for positives
//returns the amount of lower zero bits in a row
int nth_bit(size_t num) {
    uint64_t n = 1;
    int res = 0;
    while (!(n & num) || res == 63) {
        n <<= 1;
        res++;
    }
    return res;
}

size_t align_to_page(size_t target_size, size_t page_size) {
    int bits_to_clear = nth_bit(page_size);
    uint64_t mask = 0xffffffffffffffff;
    uint64_t clear_mask = 1;
    bool incr = false;
    while (bits_to_clear) {
        if (target_size & clear_mask) incr = true;
        mask ^= clear_mask;
        clear_mask <<= 1;
        bits_to_clear--;
    }
    target_size &= mask;
    if (incr) target_size += page_size;
    return target_size;
}

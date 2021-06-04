#ifndef MLOG_ALLOC_H
#define MLOG_ALLOC_H

#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

static size_t array_size;
static size_t page_size;
static size_t aligned_array_size;
static bool inited = false;

//int file_ids;
//int max_files;

typedef struct mf_array mf_array;

//pos for positive
struct mf_array {
    size_t el_size;
    size_t count_pos;
    size_t alloced_pos;//needed for munmap
    uint8_t* store_pos;
};

//mapping size for mmap based arrays
//element size for malloc
uint8_t* new_store(size_t);
void free_store(uint8_t* store, size_t alloced);
mf_array* new_array(size_t el_size);
void free_array(mf_array*);

void* at(mf_array*, size_t);
bool set(mf_array*, size_t i, void* element);
int card(mf_array*);
bool init_arrays(void);

//only for positives
//returns the amount of lower zero bits in a row
int nth_bit(size_t num);
size_t align_to_page(size_t target_size, size_t page_size);

#endif
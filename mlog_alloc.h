#ifndef MLOG_ALLOC_H
#define MLOG_ALLOC_H

#include <stdint.h>
#include <stdbool.h>

//200mb for now
#define MAX_SIZE 200000000

int file_ids;
int max_files;

typedef struct mf_array mf_array;

//pos for positive
struct mf_array {
    int el_size;

    int count_pos;
    int count_neg;
    int alloced_pos;//needed for munmap
    int alloced_neg;
    uint8_t* store_pos;
    uint8_t* store_neg;
};

bool init_arrays(void);
int get_max_files(void);
void mkdir_silent(const char* name);

uint8_t* new_store(int sparse_size);
void free_store(uint8_t* store, int alloced);
mf_array* new_array(int el_size);
void free_array(mf_array*);

void* at(mf_array*, int);
bool set(mf_array*, int i, void* element);
int card(mf_array*);

#endif
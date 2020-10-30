#ifndef MAFIALOG_ALLOC_H
#define MAFIALOG_ALLOC_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

#define forever while(1)

uint file_ids;

void init_sig_handles(void);
void sig_normal_exit(int);
void sig_segfault(int);

int get_open_max(void);

typedef struct page_entry {
    long int* start_addr;
    size_t size;
} page_entry;

typedef struct array_store {
    char* name;
    page_entry* store;
    page_entry* store_negative;
    size_t array_length;
    size_t array_negative;
    bool freed;
} array_store;

enum val_opt {Value, Fail};
typedef struct value {
    enum val_opt tag;
    long int val;
} value;

value at(array_store*, int);
value set(array_store*, value, int);
value card(array_store*);

array_store* new_var(char*);
int free_var(array_store*);
void debug_var(array_store*);
void dump_var(array_store*);

page_entry* new_page(size_t sparse_size);
int free_page(page_entry*);
void debug_page(page_entry*);

char* map_sparse_file_demo(void);
void single_page_test(void);
void write_page(page_entry*);

#endif

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

extern int errno;

#include "cow_map.h"

void check_map_error(void* map) {
    if (map == NULL) {
        perror("Error. Could not create map with mmap().\n");
        exit(EXIT_FAILURE);
    }
}

//old
void check_neighbours(uint8_t* a, uint8_t* b) {
    if (a + getpagesize() != b) {
        printf("Error. Pages are not neighbours.\n");
        printf("%p : %p\n", a, b);
        exit(EXIT_FAILURE);
    }
}

void check_mapped_addr(uint8_t* target, uint8_t* mapping) {
    if (target != mapping) {
        printf("Error. addresses are not the same.\n");
        printf("%p : %p\n", target, mapping);
        exit(EXIT_FAILURE);
    }
}

cow_map* new_cow_map(int element_size) {
    cow_map* map = malloc(sizeof(cow_map));
    map->pages = 4;
    map->max_size = getpagesize()*map->pages;

    int page_size = getpagesize();
    int fd = new_anon_file(page_size);
    map->parent = mmap(
        (void*)0x7fd1038d8000,
        page_size,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        fd, 0
    );
    check_map_error(map->parent);
    
    for (int i = 1; i < map->pages; i++) {
        fd = new_anon_file(page_size);
        //uint8_t orig_addr = map->parent + (page_size*i);
        uint8_t* target = map->parent + (page_size*i);
        uint8_t* map_res = mmap(
            target,
            page_size,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            fd, 0
        );
        check_map_error(map_res);
        check_mapped_addr(target, map_res);
    }
    
    map->dirty = new_vector(0, sizeof(int));
    map->element_size = element_size;
    return map;
}

cow_map* new_cow_map_old(int element_size) {
    cow_map* map = malloc(sizeof(cow_map));
    map->max_size = getpagesize()*4;
    int fd = new_anon_file(map->max_size);
    map->parent = mmap(
        NULL,
        map->max_size,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        fd, 0
    );
    if (map->parent == NULL) {
        perror("Error. Could not create map with mmap().\n");
        exit(EXIT_FAILURE);
    }
    map->dirty = new_vector(0, sizeof(int));
    map->element_size = element_size;
    return map;
}

cow_map* duplicate_cow_map(cow_map* mp) {
    cow_map* map = malloc(sizeof(cow_map));
    map->max_size = mp->max_size;
    //map->parent = mp->parent;
    //need to mremap here
    uint8_t* result_map = mremap(
        mp->parent,
        0,
        mp->max_size,
        MREMAP_MAYMOVE
    );
    if (result_map == MAP_FAILED) {
        perror("Error. mremap() failed in duplicate_cow_map.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("result_map : %p\n", result_map);
    }
    map->parent = result_map;
    map->element_size = mp->element_size;
    map->dirty = new_vector(0, sizeof(int));
    return map;
}

bool cow_map_insert(cow_map* mp, void* el, int addr) {
    if (addr*mp->element_size >= mp->max_size) return false;
    int page_size = getpagesize();
    int page_no = addr/page_size;//rounded down?
    if (vec_contains(mp->dirty, &page_no)) {
        //printf("Info :: insert(), page %i already dirty, no need to copy\n", page_no);
        //already dirty, no need to copy
        uint8_t* element = el;
        for (int i = 0; i < mp->element_size; i++) {
            mp->parent[(addr*mp->element_size)+i] = element[i];
        }
    } else {
        printf("Info :: insert(), copying page %i to insert\n", page_no);
        //pristine, need to copy
        uint8_t backup[page_size];
        uint8_t* target_page = mp->parent+(page_no*page_size);
        for (int i = 0; i < page_size; i++) {
            //backup[i] = mp->parent[(page_no*page_size)+i];
            backup[i] = target_page[i];
        }
        munmap(target_page, page_size);
        int fd = new_anon_file(page_size);
        uint8_t* result_map = mmap(
            target_page,
            page_size,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            fd, 0
        );
        if (result_map == MAP_FAILED) {
            perror("Error. mmap() failed in insert() :\n");
            return false;
        } else if (result_map != target_page) {
            printf("Error. mmap() mapped a page in the wrong spot\n");
            return false;
        }
        //move back backup page
        for (int i = 0; i < page_size; i++) {
            target_page[i] = backup[i];
        }
        //mark dirty
        vec_push_back(mp->dirty, &page_no);
    }
    return true;
}

void* cow_map_at(cow_map* mp, int addr) {
    return mp->parent+(addr*mp->element_size);    
}

int new_anon_file(int size) {
    char* name = unique_name();
    int fd = memfd_create(name, 0);
    free(name);
    if (fd == -1) {
        perror("Error. memfd_create() failed:\n");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, size) == -1) {
        perror("Error. ftruncate() failed:\n");
        exit(EXIT_FAILURE);
    }
    return fd;
}

//mallocs a new string
char* unique_name(void) {
    char* name = malloc(sizeof(char)*digits(unique_num)+1);
    sprintf(name, "%i", unique_num);
    unique_num++;
    return name;
}

void cow_map_info(cow_map* mp) {
    printf("\tSpans %p : %p\n", mp->parent, mp->parent+mp->max_size);
    printf("\tPages : %i\n", mp->max_size/getpagesize());
    printf("\tDirty pages : %li\n", vec_size(mp->dirty));
}

int digits(int num) {
    int res = 0;
    do {res++;} while (num/=10);
    return res;
}

int digits_neg(int num) {
    int res = num >= 0 ? 0 : 1;
    do {res++;} while (num/=10);
    return res;    
}

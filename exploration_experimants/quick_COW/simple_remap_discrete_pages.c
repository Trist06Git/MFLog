
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

static int unique_num = 0;

int new_anon_file(int size);
int new_sparse_file(int size);
void check_map_error(void* map);
char* unique_name(void);
int digits(int num);

int main(int argc, char** argv) {
    printf("Started.\n");

    int page_size = getpagesize();
    int page1_fd = new_sparse_file(page_size);
    u_int8_t* page1 = mmap(
        (void*)0x7f6fee82e000 + 4096,
        page_size,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        page1_fd,
        0
    );
    check_map_error(page1);

    int page2_fd = new_sparse_file(page_size);
    u_int8_t* page2 = mmap(
        page1+page_size,
        page_size,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        page2_fd,
        0
    );
    check_map_error(page2);
    if (page1+page_size != page2) {
        printf("Error. Pages not next to eachother.\n");
        printf("%p : %p\n", page1+page_size, page2);
        exit(1);
    }

    for (int i = 0; i < page_size*2; i++) {
        page1[i] = 7;
    }
    for (int i = 0; i < page_size*2; i++) {
        if (page1[i] != 7) {
            printf("Error. Wrong value read back: %i : %i\n", page1[i], i);
            exit(1);
        }
    }

    u_int8_t* remapped = mremap(
        page1,
        0,
        page_size*2,
        MREMAP_MAYMOVE
    );
    check_map_error(remapped);
    for (int i = 0; i < page_size*2; i++) {
        remapped[i] = 8;
    }
    for (int i = 0; i < page_size*2; i++) {
        if (remapped[i] != 7) {
            printf("Error. Wrong value read back: %i : %i\n", remapped[i], i);
            exit(1);
        }
    }

    printf("Done.\n");
    return 0;
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

int new_sparse_file(int size) {
    char* name = unique_name();
    FILE* fp = fopen(name, "w+");
    if (fp == NULL) {
        perror("Error. Could not open/create file:\n");
        exit(1);
    }
    int fd = fileno(fp);
    free(name);
    if (fd == -1) {
        perror("Error. fileno() failed:\n");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, size) == -1) {
        perror("Error. ftruncate() failed:\n");
        exit(EXIT_FAILURE);
    }
    return fd;
}

void check_map_error(void* map) {
    if (map == NULL) {
        perror("Error. Could not create map with mmap().\n");
        exit(EXIT_FAILURE);
    }
}

//mallocs a new string
char* unique_name(void) {
    char* name = malloc(sizeof(char)*digits(unique_num)+1);
    sprintf(name, "%i", unique_num);
    unique_num++;
    return name;
}

int digits(int num) {
    int res = 0;
    do {res++;} while (num/=10);
    return res;
}

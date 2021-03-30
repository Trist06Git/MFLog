
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <errno.h>

extern int errno;

int main(int argc, char** argv) {
    printf("Starting.\n");
    printf("process ID: %i\n", getpid());
    int page_size = getpagesize();
    int total_size = page_size*4;

    char* name = "original";
    int fd = memfd_create(name, 0);
    if (fd == -1) {
        perror("Error. memfd_create() failed:\n");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, total_size) == -1) {
        perror("Error. ftruncate() failed:\n");
        exit(EXIT_FAILURE);
    }

    uint8_t* mem_orig = mmap(
        NULL,
        total_size,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        fd, 0
    );
    if (mem_orig == MAP_FAILED) {
        perror("Error. mmap() failed:\n");
        exit(EXIT_FAILURE);
    }
    printf("Filling original with 7s\n");
    for (int i = 0; i < total_size; i++) {
        mem_orig[i] = 7;
    }

    uint8_t* mem_copy1 = mmap(
        NULL,
        total_size,
        PROT_READ|PROT_WRITE,
        MAP_PRIVATE,
        fd, 0
    );
    if (mem_copy1 == MAP_FAILED) {
        perror("Error. mmap() failed:\n");
        exit(EXIT_FAILURE);
    }
    printf("Filling copys first page with 8s\n");
    for (int i = 0; i < page_size; i++) {
        mem_copy1[i] = 8;
    }
    printf("Checking copy\n");
    for (int i = 0; i < total_size; i++) {
        if (i < page_size && mem_copy1[i] != 8) {
            printf("Error. Invalid value in mem_copy1 at %i : %i\n", i, mem_copy1[i]);
            break;
        } else if (i >= page_size && mem_copy1[i] != 7) {
            printf("Error. Invalid value in mem_copy1 at %i : %i\n", i, mem_copy1[i]);
            break;
        }
    }

    uint8_t* mem_copy2 = mmap(
        NULL,
        total_size,
        PROT_READ|PROT_WRITE,
        MAP_PRIVATE,
        fd, 0
    );
    if (mem_copy2 == MAP_FAILED) {
        perror("Error. mmap() failed:\n");
        exit(EXIT_FAILURE);
    }
    printf("Filling copy2s last page with 6s\n");
    for (int i = total_size-page_size; i < total_size; i++) {
        mem_copy2[i] = 6;
    }
    printf("Checking copy2\n");
    for (int i = 0; i < total_size; i++) {
        if (i < total_size-page_size && mem_copy2[i] != 7) {
            printf("Error. Invalid value in mem_copy2 at %i : %i\n", i, mem_copy2[i]);
            break;
        } else if (i >= total_size-page_size && mem_copy2[i] != 6) {
            printf("Error. Invalid value in mem_copy2 at %i : %i\n", i, mem_copy2[i]);
            break;
        }
    }

    printf("Checking original\n");
    //check original
    for (int i = 0; i < total_size; i++) {
        if (mem_orig[i] != 7) {
            printf("Error. Invalid value in mem_orig at %i : %i\n", i, mem_orig[i]);
            break;
        }
    }

    int fd2 = memfd_create("jump", 0);
    if (fd2 == -1) {
        perror("Error. memfd_create() failed:\n");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd2, total_size) == -1) {
        perror("Error. ftruncate() failed:\n");
        exit(EXIT_FAILURE);
    }
    uint8_t* mem_jump = mmap(
        mem_copy1,
        total_size,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        fd2, 0
    );
    if (mem_jump == MAP_FAILED) {
        perror("Error. mmap()ing mem_jump failed:\n");
        exit(EXIT_FAILURE);
    }
    if (mem_jump != mem_copy1) {
        printf("NOTE: mem_jump is not the same address as mem_copy1\n");
    }

    uint8_t* mem_copy3 = mmap(
        NULL,
        total_size,
        PROT_READ|PROT_WRITE,
        MAP_PRIVATE,
        fd2, 0
    );
    if (mem_copy3 == MAP_FAILED) {
        perror("Error. mmap()ing mem_copy2 failed:\n");
        exit(EXIT_FAILURE);
    }
    printf("Filling mem_copy3s second page with 5s\n");
    for (int i = page_size; i < page_size*2; i++) {
        mem_copy3[i] = 5;
    }
    printf("Checking mem_copy3\n");
    for (int i = 0; i < total_size; i++) {
        if (i < page_size && mem_copy3[i] != 8) {
            printf("Error. Invalid value in mem_copy3 at %i : %i\n", i, mem_copy3[i]);
            break;
        } else if (i >= page_size && i < page_size*2 && mem_copy3[i] != 6) {
            printf("Error. Invalid value in mem_copy3 at %i : %i\n", i, mem_copy3[i]);
            break;
        } else if (i >= page_size*2 && mem_copy3[i] != 7) {
            printf("Error. Invalid value in mem_copy3 at %i : %i\n", i, mem_copy3[i]);
            break;
        }
    }

    printf("Checking mem_copy1\n");
    for (int i = 0; i < total_size; i++) {
        if (i < page_size && mem_copy1[i] != 8) {
            printf("Error. Invalid value in mem_copy1 at %i : %i\n", i, mem_copy1[i]);
            break;
        } else if (i >= page_size && mem_copy1[i] != 7) {
            printf("Error. Invalid value in mem_copy1 at %i : %i\n", i, mem_copy1[i]);
            break;
        }
    }

    getc(stdin);

    //also test read after munmap!
    munmap(mem_orig, total_size);
    munmap(mem_copy1, total_size);
    munmap(mem_copy2, total_size);
    munmap(mem_jump, total_size);

    printf("Done.\n");
    return 0;
}

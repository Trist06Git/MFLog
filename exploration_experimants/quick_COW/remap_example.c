
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

static int unique_num = 0;

int new_anon_file(int size);
void check_map_error(void* map);
char* unique_name(void);
int digits(int num);

int main(int argc, char** argv) {
    printf("Started.\n");

    int page_size = getpagesize();
    int fd = new_anon_file(page_size*3);
    u_int8_t* mapping = mmap(
        NULL,
        page_size*3,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        fd,0
    );
    check_map_error(mapping);
    
    u_int8_t* second = mremap(
        mapping,
        0,
        page_size*3,
        MREMAP_MAYMOVE
    );
    check_map_error(second);

    for (int i = 0; i < page_size*3; i++) {
        mapping[i] = 7;
    }
    for (int i = 0; i < page_size*3; i++) {
        if (second[i] != 7) {
            printf("Error. Invalid value read back. %i : %i\n", second[i], i);
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

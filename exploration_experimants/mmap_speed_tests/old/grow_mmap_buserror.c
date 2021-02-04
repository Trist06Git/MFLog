
#include <stdio.h>
#include <stdint.h>
#define __USE_GNU
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

extern int errno;

//realloc vs mremap, as per manual pages who claim mremap its faster...
//and indeed it is faster.
int main(int argc, char** argv) {
    printf("Starting.\n");
    printf("process id: %i\n", getpid());

    size_t page_size = getpagesize();

    uint8_t* chunk = mmap(
        NULL,
        page_size,
        PROT_READ|PROT_WRITE|PROT_EXEC,
        MAP_ANONYMOUS|MAP_SHARED,////////change to MAP_PRIVATE to fix...
        -1,                       //fd
        0                        //offset
    );
    if (chunk == MAP_FAILED) {
        perror("Error. mremap() faild. Exiting now\n");
        exit(EXIT_FAILURE);
    }
    for (int j = 0; j < page_size; j++) {
        chunk[j] = 7;
    }

    size_t two_pages = page_size*2;
    chunk = mremap(
        chunk,                //old addr
        page_size,           //old size
        two_pages,          //new size
        MREMAP_MAYMOVE
    );
    if (chunk == MAP_FAILED) {
        perror("Error. mremap() faild. Exiting now\n");
        exit(EXIT_FAILURE);
    }
    printf("expanded to %lu\n", two_pages);
    for (int j = 0; j < two_pages; j++) {
        chunk[j] = 8;
    }

    printf("Done. Press enter to free memory and quit.\n");
    getc(stdin);
    munmap(chunk, page_size*2);

    return 0;
}

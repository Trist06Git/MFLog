
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "mlog_alloc.h"

extern int file_ids;
extern int max_files;

//NULL is failure
mf_array* new_array(int el_size) {
    if (file_ids >= max_files-4) {//-4 for some reason..
        printf("Error, The limit of open files has been reached (%i/%i)\n", file_ids, max_files);
        printf("       please adjust the limit with \"ulimit -Sn\".\n");
        return NULL;
    }

    mf_array* arr = malloc(sizeof(mf_array));
    arr->store_pos = new_store(MAX_SIZE);
    if (arr->store_pos == NULL) {
        printf("Error, new_store() pos failed.\n");
        free(arr);
        return NULL;
    }
    arr->store_neg = new_store(MAX_SIZE);
    if (arr->store_neg == NULL) {
        printf("Error, new_store() neg failed.\n");
        free_store(arr->store_pos, arr->alloced_pos);
        free(arr);
        return NULL;
    }

    arr->el_size = el_size;
    arr->count_pos = 0;
    arr->count_neg = 0;
    arr->alloced_pos = MAX_SIZE;
    arr->alloced_neg = MAX_SIZE;

    return arr;
}

void free_array(mf_array* arr) {
    free_store(arr->store_pos, arr->alloced_pos);
    free_store(arr->store_neg, arr->alloced_neg);
    free(arr);
}

uint8_t* new_store(int to_alloc) {
    char fname[80];
    sprintf(fname, "vars/var_%i", file_ids++);
    int fd = open(fname, O_RDWR|O_CREAT, 0666);
    if (fd < 0) {
        perror("Error, open() failed");
        return NULL;
    }
    //make sparse
    write(fd, "8", 1);
    lseek(fd, to_alloc-2, SEEK_CUR);
    write(fd, "8", 1);
    
    uint8_t* store = mmap(
        NULL,                           //dont care where the mapping is
        to_alloc,                
        PROT_READ|PROT_WRITE|PROT_EXEC, //protections
        MAP_SHARED,           
        fd,                             
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

void free_store(uint8_t* store, int alloced) {
    int result = munmap(store, alloced);
    if (result) {
        perror("Error, munmap() failed");
    }
}

void* at(mf_array* arr, int i) {
    if (i < arr->count_pos + arr->count_neg) {
        int real_loc = i - arr->count_neg;
        if (real_loc < 0) {
            real_loc++;
            real_loc = -real_loc;
            //printf("at real loc calculated as %i, fake: %i\n", real_loc, i);
            return arr->store_neg + (real_loc*arr->el_size);
        } else {
            return arr->store_pos + (real_loc*arr->el_size);
        }
    } else {
        return NULL;
    }
}

//false/0 is failure
bool set(mf_array* arr, int i, void* element) {
    if (i > arr->count_pos + arr->count_neg + 1) {
        return false;
    } else if (i < -1) {
        return false;
    } else {
        int real_loc = i - arr->count_neg;
        if (real_loc < 0) {
            real_loc++;
            real_loc = -real_loc;
            for (int j = 0; j < arr->el_size; j++) {//j bytes long
                uint8_t* i_element = element;
                arr->store_neg[(real_loc*arr->el_size) + j] = i_element[j];
            }
            if (real_loc == arr->count_neg) arr->count_neg++;//push front
            return true;

        } else {
            for (int j = 0; j < arr->el_size; j++) {//j bytes long
                uint8_t* i_element = element;
                arr->store_pos[(real_loc*arr->el_size) + j] = i_element[j];
            }
            if (real_loc == arr->count_pos) arr->count_pos++;//append
            return true;
        }
    }
}

int card(mf_array* arr) {
    return arr->count_pos + arr->count_neg;
}

bool init_arrays(void) {
    file_ids = 0;
    max_files = get_max_files();
    mkdir_silent("");
}

int get_max_files(void) {
    FILE* fp = popen("getconf OPEN_MAX","r");
    if (fp == NULL) {
        perror("could not query max open files : \"getconf OPEN_MAX\"");
        return -1;
    }

    int result = 0;
    fscanf(fp, "%i", &result);
    fclose(fp);
    return result;
}

void mkdir_silent(const char* name) {
    FILE* fp = popen("mkdir vars &>/dev/null","r");
    if (fp == NULL) {
        perror("could not query the results from : \"mkdir vars\"");
        return;
    }
    fclose(fp);
}

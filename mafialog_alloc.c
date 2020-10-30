
#include "mafialog_alloc.h"

value at(array_store* var, int index) {
    value result;
    if (index >= 0 && index < var->array_length) {//for now only positive indicies
        result.tag = Value;
        result.val = var->store->start_addr[index];
    } else if (index < 0 && index > -var->array_negative) {
        index += 1;//add one for first neg index -1 -> 0
        result.tag = Value;
        result.val = var->store_negative->start_addr[-index];
    } else {
        //printf("Error! At from %s failed at index %i\n", var->name, index);
        //printf("length : %zu\n", var->array_length);
        result.tag = Fail;
    }
    return result;
}

value set(array_store* var, value val, int index) {
    if (index > 0 && index < var->array_length) {//set in middle
        var->store->start_addr[index] = val.val;
    } else if (index == var->array_length) {//push end
        var->array_length++;
        var->store->start_addr[index] = val.val;
    } else if (index < 0 && index > -var->array_negative) {
        index += 1;//add one for first neg index -1 -> 0
        var->store_negative->start_addr[-index] = val.val;
    } else if (index == -var->array_negative) {//push front
        index += 1;
        if (var->store_negative == NULL) {
            var->store_negative = new_page(200000000);
        }
        var->array_negative++;
        var->store_negative->start_addr[-index] = val.val;
    } else {
        val.tag = Fail;
    }
    return val;
}

array_store* new_var(char* name) {
    array_store* var = malloc(sizeof(array_store));
    var->name = name;
    var->store = new_page(200000000);
    var->array_length = 0;
    var->store_negative = NULL;
    var->array_negative = 1;
    var->freed = false;

    return var;
}

int free_var(array_store* var) {
    if (var->store != NULL) free_page(var->store);
    if (var->store_negative != NULL) free_page(var->store_negative);
    free(var);
    
    return 0;
}

void dump_var(array_store* var) {
    printf("%s :: dump\n", var->name);
    for (int i = 0; i < var->array_length; i++) {
        printf("%lu ", var->store->start_addr[i]);
        if (i % 8 == 7) printf("\n");
    }
}

void debug_var(array_store* var) {
    printf("%s :: info\n", var->name);
    printf("used : %zu\n", var->array_length);
    printf("allocated : %zu\n", var->store->size);
    printf("addr : %p\n", var->store->start_addr);
    printf("%s marked for collection\n", var->freed ? "":"not ");
}

page_entry* new_page(size_t sparse_size) {
    char fname[80];
    sprintf(fname, "var_%i", file_ids++);
    int fd = open(fname, O_RDWR|O_CREAT, 0666);
    if (fd < 0) {
        perror("Error. open() failed");
        exit(EXIT_FAILURE);
    }
    //make sparse
    write(fd, "8", 1);
    lseek(fd, sparse_size-2, SEEK_CUR);
    write(fd, "8", 1);
    
    page_entry* new_entry = malloc(sizeof(page_entry));
    new_entry->size = sparse_size/sizeof(unsigned long int);//default type u l int, 8bytes
    new_entry->start_addr = mmap(
        NULL,                           //dont care where the mapping is
        sparse_size,                
        PROT_READ|PROT_WRITE|PROT_EXEC, //protections
        MAP_SHARED,           
        fd,                             
        0                               //offset into mapped file, 0 for anon
    );
    if (new_entry->start_addr == MAP_FAILED) {
        perror("Error. mmap() failed");
        exit(EXIT_FAILURE);
    } else {
        //printf("mapped to address: %p\n", new_entry->start_addr);
    }
    return new_entry;
}

int free_page(page_entry* entry) {
    int result = munmap(entry->start_addr, entry->size);
    if (result) {
        perror("Error. munmap() failed");
        exit(EXIT_FAILURE);
    } else {
        free(entry);
        return result;
    }
}

void debug_page(page_entry* page) {
    if (page == NULL) {
        printf("page has already been freed\n");
    } else {
        printf("page is %zu long and mapped at %p\n", page->size, page->start_addr);
    }
}

void write_page(page_entry* page) {
    printf("test writing to page\n");
    for (size_t i = 0; i < page->size; i++) {
        page->start_addr[i] = i;
    }
    printf("done test writing to page\n");
}

void single_page_test(void) {
    printf("%s\n", "::Single page test::");

    size_t pagesize = getpagesize();
    printf("page size: %zu\n", pagesize);

    size_t req_addr = (1<<20)*pagesize;
    printf("%s\n", "mmapping static page..");
    char* vm_mem = mmap(
        NULL,                           //done care where the mapping is
        pagesize,                       //length, one page size worth
        PROT_READ|PROT_WRITE|PROT_EXEC, //protections
        MAP_ANON|MAP_PRIVATE,           //anonynous, no file backing
        -1,                             //file descriptor, none, -1 for anon
        0                               //offset into mapped file, 0 for anon
    );
    if (vm_mem == MAP_FAILED) {
        perror("mmap failed..");
        exit(EXIT_FAILURE);
    } else {
        printf("mapped to address: %p\nrequested address: 0x%lx\n", vm_mem, req_addr);
    }

    return;
}

/*
<< 13, 8k
sparse files start from 8k onwards in ext4 it seems
default 200m for now..
*/
char* map_sparse_file_demo() {
    //size_t size = 1 << 16;
    size_t size = 200000000;
    int fd = open("test.page", O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("open() failed..");
        exit(EXIT_FAILURE);
    }

    //make sparse
    write(fd, "8", 1);
    lseek(fd, size-2, SEEK_CUR);
    write(fd, "8", 1);

    char* addr = mmap(
        NULL,
        size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0
    );
    if (addr == MAP_FAILED) {
        perror("mmap() failed..");
        exit(EXIT_FAILURE);
    }
    return addr;
}

void sig_normal_exit(int sig) {
    printf("Signal %i caught, exiting normally.\n", sig);
    exit(EXIT_SUCCESS);
}

void sig_segfault(int sig) {
    printf("!!Segfault %i caught, exiting!!\n", sig);
    exit(EXIT_FAILURE);
}

void init_sig_handles(void) {
    //signal(SIGINT , sig_normal_exit);
    //signal(SIGABRT, sig_normal_exit);
    //signal(SIGILL , sig_normal_exit);
    //signal(SIGFPE , sig_normal_exit);
    signal(SIGSEGV, sig_segfault);
    //signal(SIGTERM, sig_normal_exit);
    return;
}

int get_open_max(void) {
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

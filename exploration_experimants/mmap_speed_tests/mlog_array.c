
#include <string.h>

#include "mlog_array.h"

//pop does a copy, then removes from the array
//the caller needs to free the item once finished with it
void* mfa_pop_back(mf_array* arr) {
    uint8_t* res = malloc(arr->el_size);
    uint8_t* item = mfa_at(arr, mfa_card(arr)-1);
    if (item == NULL) return NULL;
    memcpy(res, item, arr->el_size);
    mfa_remove_back(arr);
    return res;
}

//pop does a copy, then removes from the array
//the caller needs to free the item once finished with it
void* mfa_pop_front(mf_array* arr) {
    uint8_t* res = malloc(arr->el_size);
    uint8_t* item = mfa_at(arr, 0);
    if (item == NULL) return NULL;
    memcpy(res, item, arr->el_size);
    mfa_remove_front(arr);
    return res;
}

bool mfa_push_back(mf_array* arr, void* element) {
    return mfa_set(arr, mfa_card(arr), element);
}

bool mfa_push_front(mf_array* arr, void* element) {
    return mfa_set(arr, -1, element);
}

bool mfa_remove_back(mf_array* arr) {
    return mfa_remove_at(arr, mfa_card(arr)-1);
}

bool mfa_remove_front(mf_array* arr) {
    return mfa_remove_at(arr, 0);
}

//set copies in the element
//false\0 is failure//maybe make it -1??
//also consider removing from front > count_pos.
//"set" is in reference to "setting" the current index to val..
bool mfa_set(mf_array* arr, long int i, void* element) {
    if (i < -1 || i > arr->count_pos+arr->count_neg) {
        return false;
    } else if (i == -1) {//push front
        //need to check precalc_alloced limit
        for (int j = 0; j < arr->el_size; j++) {
            uint8_t* sized_el = element;
            arr->store_neg[(arr->count_neg*arr->el_size) + j] = sized_el[j];
        }
        arr->count_neg++;
        return true;

    } else if (i == arr->count_pos+arr->count_neg) {//push back//was alloced_neg
        for (int j = 0; j < arr->el_size; j++) {
            uint8_t* sized_el = element;
            arr->store_pos[(arr->count_pos*arr->el_size) + j] = sized_el[j];//why way this + i??
        }
        arr->count_pos++;
        return true;
    } else {
        int real_i = i - arr->count_neg;
        if (real_i < 0) {//neg
            real_i++;
            real_i = abs(real_i);
            for (int j = 0; j < arr->el_size; j++) {
                uint8_t* sized_el = element;
                arr->store_neg[(real_i*arr->el_size) + j] = sized_el[j];
            }
            //arr->count_neg++;//Not expanding
            return true;
        } else {//pos
            for (int j = 0; j < arr->el_size; j++) {
                uint8_t* sized_el = element;
                arr->store_pos[(real_i*arr->el_size) + j] = sized_el[j];
            }
            //arr->count_pos++;//Not expanding
            return true;
        }
    }
    return false;
}

//at returns a pointer to the underlying data item in the array
//NULL is fail//looks kind of nasty, refactor?
void* mfa_at(mf_array* arr, long int i) {
    if (i < 0 || i > arr->count_neg + arr->count_pos - 1) {
        return NULL;
    } else {
        long int real_i = i - arr->count_neg;
        if (real_i < 0) {//in neg
            real_i++;
            real_i = abs(real_i);
            if (real_i < arr->count_neg) {
                return arr->store_neg + (real_i*arr->el_size);
            } else {
                return NULL;
            }
        } else {//in pos
            if (real_i < arr->count_pos) {
                return arr->store_pos + (real_i*arr->el_size);
            } else {
                return NULL;
            }
        }
    }
}

bool mfa_remove_at(mf_array* arr, long int i) {
    if (i < 0 || i > arr->count_neg + arr->count_pos - 1) {
        return false;
    } else {
        long int real_i = i - arr->count_neg;
        if (real_i < 0) {//in neg
            real_i++;
            real_i = abs(real_i);
            if (real_i < arr->count_neg) {
                //shuffle them along
                real_i++;
                for (; real_i < arr->count_neg; real_i++) {
                    for (int j = 0; j < arr->el_size; j++) {//for each byte
                        arr->store_neg[(real_i-1)*arr->el_size + j]
                          =
                        arr->store_neg[(real_i-0)*arr->el_size + j];
                    }
                }
                arr->count_neg--;
                return true;
            } else {
                return false;
            }
        } else {//in pos
            if (real_i < arr->count_pos) {
                real_i++;
                for (; real_i < arr->count_pos; real_i++) {
                    for (int j = 0; j < arr->el_size; j++) {//for each byte
                        arr->store_pos[(real_i-1)*arr->el_size + j]
                          =
                        arr->store_pos[(real_i-0)*arr->el_size + j];
                    }
                }
                arr->count_pos--;
                return true;
            } else {
                return false;
            }
        }
    }
}

size_t mfa_card(mf_array* arr) {
    return arr->count_neg + arr->count_pos;
}

//this needs to be replaced with COW/copy on write
mf_array* mfa_duplicate(mf_array* arr) {
    mf_array* ret = new_mfarray(arr->el_size);
    for (int i = 0; i < arr->count_pos; i++) {
        for (int j = 0; j < arr->el_size; j++) {
            ret->store_pos[(i*arr->el_size)+j] = arr->store_pos[(i*arr->el_size)+j];
        }
    }
    for (int i = 0; i < arr->count_neg; i++) {
        for (int j = 0; j < arr->el_size; j++) {
            ret->store_neg[(i*arr->el_size)+j] = arr->store_neg[(i*arr->el_size)+j];
        }
    }
    ret->alloced_pos = arr->alloced_pos;
    ret->alloced_neg = arr->alloced_neg;
    ret->count_pos = arr->count_pos;
    ret->count_neg = arr->count_neg;
    ret->precalc_alloced_pos = arr->precalc_alloced_pos;
    ret->precalc_alloced_neg = arr->precalc_alloced_neg;
    return ret;
}

//byte compare, false = different, true = same contents
bool mfa_compare(mf_array* arr1, mf_array* arr2) {
    if (arr1->el_size != arr2->el_size
          ||
        mfa_card(arr1) != mfa_card(arr2)
    ) return false;

    for (int i = 0; i < mfa_card(arr1); i++) {
        uint8_t* el1 = mfa_at(arr1, i);
        uint8_t* el2 = mfa_at(arr2, i);
        for (int j = 0; j < arr1->el_size; j++) {
            if (el1[j] != el2[j]) return false;
        }
    }
    return true;
}

void mfa_set_comparator(mf_array* arr, int (*comp)(const void*, const void*, int)) {
    arr->comparator = comp;
}

#define TEST_MAX 4294967296*2

//NULL is failure
mf_array* new_mfarray(size_t el_size) {
    if (!inited) init_mfarrays();
    mf_array* arr = malloc(sizeof(mf_array));
    //size_t aligned = align_to_page(init_array_size, page_size);
    size_t aligned = align_to_page(TEST_MAX, page_size);
    arr->store_pos = new_store(aligned);
    if (arr->store_pos == NULL) {
        printf("Error, new_store() pos failed.\n");
        free(arr);
        return NULL;
    }
    arr->store_neg = new_store(aligned);
    if (arr->store_neg == NULL) {
        printf("Error, new_store() neg failed.\n");
        free_store(arr->store_pos, arr->alloced_pos);
        free(arr);
        return NULL;
    }

    arr->el_size = el_size;
    arr->count_pos           = arr->count_neg           = 0;
    arr->alloced_pos         = arr->alloced_neg         = aligned;
    arr->precalc_alloced_pos = arr->precalc_alloced_neg = aligned / el_size;

    return arr;
}

void free_mfarray(mf_array* arr) {
    free_store(arr->store_pos, arr->alloced_pos);
    free(arr);
}

//NULL is failure
uint8_t* new_store(size_t to_alloc) {
    uint8_t* store = mmap(
        NULL,                           //dont care where the mapping is
        to_alloc,                
        PROT_READ|PROT_WRITE|PROT_EXEC, //protections
        MAP_ANONYMOUS|MAP_PRIVATE,           
        -1,                             
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

void free_store(uint8_t* store, size_t alloced) {
    int result = munmap(store, alloced);
    if (result) {
        perror("Error, munmap() failed");
    }
}

bool init_mfarrays(void) {
    //manually fetched from getrlimit(), see user_vm_space_stats.c for details
    init_array_size = sqrt(18446744073709551615.0);
    page_size = getpagesize();
    aligned_array_size = align_to_page(init_array_size, page_size);
    inited = true;
    //no false outcomes yet..
    return true;
}

//only for positives
//returns the amount of lower zero bits in a row
int nth_bit(size_t num) {
    uint64_t n = 1;
    int res = 0;
    while (!(n & num) || res == 63) {
        n <<= 1;
        res++;
    }
    return res;
}

//could probably do better
size_t align_to_page(size_t target_size, size_t page_size) {
    int bits_to_clear = nth_bit(page_size);
    uint64_t mask = 0xffffffffffffffff;
    uint64_t clear_mask = 1;
    bool incr = false;
    while (bits_to_clear) {
        if (target_size & clear_mask) incr = true;
        mask ^= clear_mask;
        clear_mask <<= 1;
        bits_to_clear--;
    }
    target_size &= mask;
    if (incr) target_size += page_size;
    return target_size;
}

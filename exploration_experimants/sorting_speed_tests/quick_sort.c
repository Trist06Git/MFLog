
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "generic_vector.h"
#include "mlog_array.h"
#include "csv_append.h"
#include "proc_self.h"
#include "self_reboot.h"

//#define TEST_SIZE 1000000
//#define TEST_SIZE 10000000
//#define REPEATE_COUNT 25
//#define STORAGE_VEC
//#define STORAGE_MFA

void swap(int* a, int* b) {
    int temp = *a;
   *a = *b;
   *b = temp;
}

int* arr_partition(int* begin, int* end) {
    int pivot = *end;
    int* i = begin;
    for (int* j = begin; j < end; j++) {
        if (*j < pivot) {
            swap(i, j);
            i++;
        }
    }
    swap(i, end);
    return i;
}

//end is inclusive
void arr_quick_sort(int* begin, int* end) {
    if (begin < end) {
        int* part = arr_partition(begin, end);
        arr_quick_sort(begin, part-1);
        arr_quick_sort(part+1, end);
    }
}

void vec_swap(void* a, void* b, int size) {
    u_int8_t* aa = a;
    u_int8_t* bb = b;
    for (int i = 0; i < size; i++) {
        u_int8_t temp = aa[i];
        aa[i] = bb[i];
        bb[i] = temp;
    }
}

void* vec_partition(void* begin, void* end, vector* root) {
    void* pivot = end;
    u_int8_t* i = begin;
    for (u_int8_t* j = begin; j < (u_int8_t*)end; j += root->el_size) {
        if (root->comparator(j, pivot, root->el_size) < 0) {
            vec_swap(i, j, root->el_size);
            i += root->el_size;
        }
    }
    vec_swap(i, end, root->el_size);
    return i;
}

void vec_quick_sort(void* begin, void* end, vector* root) {
    if ((u_int8_t*)begin < (u_int8_t*)end) {
        u_int8_t* part = vec_partition(begin, end, root);
        vec_quick_sort(begin, part-root->el_size, root);
        vec_quick_sort(part+root->el_size, end, root);
    }
}

void vec_quick_sort_entry(vector* vec) {
    u_int8_t* start = vec->store;
    u_int8_t* end = start + vec->count * vec->el_size;
    vec_quick_sort(vec->store, end, vec);
}

void* mfa_partition(void* begin, void* end, mf_array* root) {
    void* pivot = end;
    u_int8_t* i = begin;
    for (u_int8_t* j = begin; j < (u_int8_t*)end; j += root->el_size) {
        if (root->comparator(j, pivot, root->el_size) < 0) {
            vec_swap(i, j, root->el_size);
            i += root->el_size;
        }
    }
    vec_swap(i, end, root->el_size);
    return i;
}

void mfa_quick_sort(void* begin, void* end, mf_array* root) {
    if ((u_int8_t*)begin < (u_int8_t*)end) {
        u_int8_t* part = mfa_partition(begin, end, root);
        mfa_quick_sort(begin, part-root->el_size, root);
        mfa_quick_sort(part+root->el_size, end, root);
    }
}

void mfa_quick_sort_entry(mf_array* arr) {
    u_int8_t* start = arr->store_pos;
    u_int8_t* end = start + arr->count_pos * arr->el_size;
    mfa_quick_sort(arr->store_pos, end, arr);
}

int int_comparator(const void* a, const void* b, int size) {
    return *(int*)a - *(int*)b;
}

int char_comparator(const void* a, const void* b, int size) {
    return *(char*)a - *(char*)b;
}

void dump_array_char(char* arr, int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%c", arr[i]);
        if (i != size-1) printf(";");
    }
    printf("]\n");
}

void dump_array(int* arr, int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%i", arr[i]);
        if (i != size-1) printf(";");
    }
    printf("]\n");
}

void dump_mfarray(mf_array* arr) {
    printf("[");
    for (int i = 0; i < mfa_card(arr); i++) {
        int* val = mfa_at(arr, i);
        printf("%i", *val);
        if (i != mfa_card(arr)-1) printf(";");
    }
    printf("]\n");
}

bool check_vector(vector* vec) {
    int* last = vec_at(vec, 0);
    for (int i = 1; i < vec_size(vec); i++) {
        int* val = vec_at(vec, i);
        if (*val < *last) return false;
        last = val;
    }
    return true;
}

bool check_mfarr(mf_array* arr) {
    int* last = mfa_at(arr, 0);
    for (int i = 1; i < mfa_card(arr); i++) {
        int* val = mfa_at(arr, i);
        if (*val < *last) return false;
        last = val;
    }
    return true;
}

//taken from stack overflow...
struct timespec diff(struct timespec start, struct timespec end) {
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec) < 0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

int digits(int num) {
    int res = 0;
    do {res++;} while (num/=10);
    return res;
}

int save_page_faults(char* filecode) {
    proc_self ps;
    ps.comm = malloc(sizeof(char)*50);
    cat_proc_self(&ps);
    char* filename = malloc(sizeof(char) * (strlen(filecode) + strlen(".faults") + 1));
    sprintf(filename, "%s.faults", filecode);
    FILE* file = fopen(filename, "a+");
    if (file == NULL) {
        perror("Error :: file %s could not be opended");
        return -1;
    }
    fprintf(file, "Major faults : %lu\n", ps.majflt);
    fprintf(file, "Minor faults : %lu\n", ps.minflt);
    fclose(file);
    return 0;
}

void quick_sort_vec_test(int elements, int repeats) {
    printf("Vector quick sort test.\n");
    
    struct timespec file_time;
    int res = clock_gettime(CLOCK_REALTIME, &file_time);
    if (res < 0) {
        printf("Error. with clock_gettime().\n");
        exit(EXIT_FAILURE);
    }
    char* filename = malloc(sizeof(char)*(5+digits(elements)+2+digits(file_time.tv_sec)+4+1));//vec_eXXX_tYYY.csv
    sprintf(filename, "vec_e%i_t%i.csv", elements, (int)file_time.tv_sec);

    for (int n = 0; n < repeats; n++) {
        sleep(5);
        printf("\nTiming iteration %i\n", n);
        struct timespec start_time;
        struct timespec end_time;
        int res = clock_gettime(CLOCK_REALTIME, &start_time);
        if (res < 0) {
            printf("Error. with clock_gettime().\n");
            exit(EXIT_FAILURE);
        }

        vector* vec = new_vector(0, sizeof(int));
        vec_set_comparator(vec, int_comparator);
        
        for (int i = 0; i < elements; i++) {
            //int val = rand() % 74 + 48;
            int val = rand() % 100;
            vec_push_back(vec, &val);
        }

        vec_quick_sort_entry(vec);
        if (check_vector(vec) == false) {
            printf("Error. Not sorted..\n");
            dump_array(vec->store, vec->count);
            exit(EXIT_FAILURE);
        }
        free_vector(vec);

        res = clock_gettime(CLOCK_REALTIME, &end_time);
        if (res < 0) {
            printf("Error. with clock_gettime().\n");
            exit(EXIT_FAILURE);
        }
        struct timespec duration = diff(start_time, end_time);
        //printf("Done, took %ld sec, %ld n-sec\n", duration.tv_sec, duration.tv_nsec);
        
        int temp = n;
        vector* times = new_vector(3, sizeof(int));
        vec_push_back(times, &temp);
        vec_push_back(times, (temp = duration.tv_sec,  &temp));
        vec_push_back(times, (temp = duration.tv_nsec, &temp));
        
        if(!append_csv(filename, times, NULL)) {
            exit(EXIT_FAILURE);
        }
        free_vector(times);
    }
    save_page_faults(filename);
    free(filename);
}

void quick_sort_mfa_test(int elements, int repeats) {
    printf("Mfarray quick sort test.\n");
    
    struct timespec file_time;
    int res = clock_gettime(CLOCK_REALTIME, &file_time);
    if (res < 0) {
        printf("Error. with clock_gettime().\n");
        exit(EXIT_FAILURE);
    }
    char* filename = malloc(sizeof(char)*(5+digits(elements)+2+digits(file_time.tv_sec)+4+1));//vec_eXXX_tYYY.csv
    sprintf(filename, "vec_e%i_t%i.csv", elements, (int)file_time.tv_sec);

    for (int n = 0; n < repeats; n++) {
        sleep(5);
        printf("\nTiming iteration %i\n", n);
        struct timespec start_time;
        struct timespec end_time;
        int res = clock_gettime(CLOCK_REALTIME, &start_time);
        if (res < 0) {
            printf("Error. with clock_gettime().\n");
            exit(EXIT_FAILURE);
        }

        //vector* vec = new_vector(0, sizeof(int));
        //vec_set_comparator(vec, int_comparator);
        mf_array* arr = new_mfarray(sizeof(int));
        mfa_set_comparator(arr, int_comparator);
        
        for (int i = 0; i < elements; i++) {
            //int val = rand() % 74 + 48;
            int val = rand() % 100;
            mfa_push_back(arr, &val);
        }

        mfa_quick_sort_entry(arr);
        if (check_mfarr(arr) == false) {
            printf("Error. Not sorted..\n");
            dump_mfarray(arr);
            exit(EXIT_FAILURE);
        }
        free_mfarray(arr);

        res = clock_gettime(CLOCK_REALTIME, &end_time);
        if (res < 0) {
            printf("Error. with clock_gettime().\n");
            exit(EXIT_FAILURE);
        }
        struct timespec duration = diff(start_time, end_time);
        //printf("Done, took %ld sec, %ld n-sec\n", duration.tv_sec, duration.tv_nsec);
        
        int temp = n;
        vector* times = new_vector(3, sizeof(int));
        vec_push_back(times, &temp);
        vec_push_back(times, (temp = duration.tv_sec, &temp));
        vec_push_back(times, (temp = duration.tv_nsec, &temp));
        
        if(!append_csv(filename, times, NULL)) {
            exit(EXIT_FAILURE);
        }
        free_vector(times);
    }
    save_page_faults(filename);
    free(filename);
}

int main(int argc, char** argv) {
    printf("Starting.\n");

    int warm_up = 10;
    printf("Sleeping for %i seconds to let the kernel settle down.\n", warm_up);
    sleep(warm_up);

    time_t t;
    srand(time(&t));
    if (argc < 3) {
        printf("Running %i tests on %i items.\n", REPEATE_COUNT, TEST_SIZE);
        #ifdef STORAGE_VEC
        quick_sort_vec_test(TEST_SIZE, REPEATE_COUNT);
        #elif defined STORAGE_MFA
        quick_sort_mfa_test(TEST_SIZE, REPEATE_COUNT);
        #endif
    } else {
        int size = 0;
        int repeat = 0;
        if(sscanf(argv[2], "%i", &size) == EOF) {
            perror("");
            exit(EXIT_FAILURE);
        }
        if (sscanf(argv[1], "%i", &repeat) == EOF) {
            perror("");
            exit(EXIT_FAILURE);
        }
        printf("Running %i tests on %i items.\n", repeat, size);
        #ifdef STORAGE_VEC
        quick_sort_vec_test(size, repeat);
        #elif defined STORAGE_MFA
        quick_sort_mfa_test(size, repeat);
        #endif
    }
    
    self_reboot();

    printf("Done.\n");
    return 0;
}


//#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <chrono>
#include <thread>
#include <sstream>

#include "generic_vector.hpp"
#include "mlog_array.hpp"
#include "csv_append.hpp"

//This is mostly taken from the c implementation. Thats why it
//does lots in the c style. eg. printf vs cout

#ifndef TEST_SIZE
  #define TEST_SIZE 1000000
#endif
#ifndef REPEATE_COUNT
  #define REPEATE_COUNT 4
#endif
#ifndef STORAGE_TYPE
  #define STORAGE_TYPE 0
#endif

template<typename T>
void swap(T* i, T* j) {
    T temp = *i;
   *i = *j;
   *j = temp;
}

template<typename T>
T* vec_partition(T* begin, T* end) {
    T pivot = *end;
    T* i = begin;
    for (T* j = begin; j < end; j++) {
        if (*j < pivot) {
            swap(i, j);
            i++;
        }
    }
    swap(i, end);
    return i;
}

template<typename T>
void vec_quick_sort(T* begin, T* end) {
    if (begin < end) {
        T* part = vec_partition(begin, end);
        vec_quick_sort(begin, part-1);
        vec_quick_sort(part+1, end);
    }
}

template<typename T>
void vec_quick_sort_entry(std::vector<T>& vec) {
    T* start = vec.data();
    T* end = start + vec.size()-1;
    vec_quick_sort(start, end);
}

void c_vec_quick_sort_entry(vector* vec) {
    //asserting int type here
    int* start = (int*)vec->store;
    int* end = start + vec->count;
    vec_quick_sort(start, end);
}

void mfa_quick_sort_entry(mf_array* arr) {
    //asserting int type here
    int* start = (int*)arr->store_pos;
    int* end = start + arr->count_pos-1;
    vec_quick_sort(start, end);
}

//int int_comparator(const void* a, const void* b, int size) {
//    return *(int*)a - *(int*)b;
//}

template<typename T>
bool check_vector(std::vector<T> vec) {
    T last = vec.at(0);
    for (int i = 1; i < vec.size(); i++) {
        T val = vec.at(i);
        if (val < last) return false;
        last = val;
    }
    return true;
}

bool check_array(int* begin, int size) {
    int last = begin[0];
    for (int i = 0; i < size; i++) {
        int val = begin[i];
        if (val < last) return false;
        last = val;
    }
    return true;
}

template<typename T>
void dump_vector(std::vector<T> vec) {
    std::cout << "[";
    for (int i = 0; i < vec.size(); i++) {
        std::cout << vec.at(i);
        if (i != vec.size()-1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

void dump_array(int* arr, int size) {
    std::cout << "[";
    for (int i = 0; i < size; i++) {
        std::cout << arr[i];
        if (i != size-1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
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

void get_time(timespec* time) {
    int res = clock_gettime(CLOCK_REALTIME, time);
    if (res < 0) {
        printf("Error. with clock_gettime().\n");
        exit(EXIT_FAILURE);
    }
}

void cppvec_test(int elements) {
    std::vector<int> vec;
    for (int i = 0; i < elements; i++) {
        vec.push_back(rand() % 100);
    }

    vec_quick_sort_entry(vec);
    if (check_vector(vec) == false) {
        printf("Error. Not sorted..\n");
        dump_vector(vec);
        exit(EXIT_FAILURE);
    }
    
    vec.clear();
    vec.shrink_to_fit();
}

void cvec_test(int elements) {
    vector* vec = new_vector(0, sizeof(int));
    for (int i = 0; i < elements; i++) {
        int val = rand() % 100;
        vec_push_back(vec, &val);
    }

    c_vec_quick_sort_entry(vec);
    if (check_array((int*)vec->store, vec->count) == false) {
        printf("Error. Not sorted..\n");
        dump_array((int*)vec->store, vec->count);
        exit(EXIT_FAILURE);
    }
}

void mfa_test(int elements) {
    mf_array* arr = new_mfarray(sizeof(int));
    for (int i = 0; i < elements; i++) {
        int val = rand() % 100;
        mfa_push_back(arr, &val);
    }

    mfa_quick_sort_entry(arr);
    if (check_array((int*)arr->store_pos, arr->count_pos) == false) {
        printf("Error. Not sorted..\n");
        dump_array((int*)arr->store_pos, arr->count_pos);
        exit(EXIT_FAILURE);
    }
    
    free_mfarray(arr);
}

void quick_sort_tests(int elements, int repeats, int test_type) {
    printf("C++ Mfa/c_vec/std::vec quick sort test.\n");
    
    struct timespec file_time;
    get_time(&file_time);

    std::stringstream filename;
    filename << "cpp_" << test_type << "_e" << elements << "_t" << (int)file_time.tv_sec << ".csv";
    //char* filename = (char*)malloc(sizeof(char)*(5+digits(elements)+2+digits(file_time.tv_sec)+4+1));//vec_eXXX_tYYY.csv
    //sprintf(filename, "vec_e%i_t%i.csv", elements, (int)file_time.tv_sec);

    for (int n = 0; n < repeats; n++) {
        std::chrono::seconds dura(5);
        std::this_thread::sleep_for(dura);
        
        printf("\nTiming iteration %i\n", n);
        struct timespec start_time;
        struct timespec end_time;
        get_time(&start_time);

        switch (test_type) {
            case 0  : mfa_test(elements);    break;
            case 1  : cvec_test(elements);   break;
            case 2  : cppvec_test(elements); break;
            default :                        break;
        }

        get_time(&end_time);
        struct timespec duration = diff(start_time, end_time);
        //printf("Done, took %ld sec, %ld n-sec\n", duration.tv_sec, duration.tv_nsec);
        
        //c_vector needed for append_csv
        int temp = n;
        vector* times = new_vector(3, sizeof(int));
        vec_push_back(times, &temp);
        vec_push_back(times, (temp = duration.tv_sec, &temp));
        vec_push_back(times, (temp = duration.tv_nsec, &temp));
        
        if(!append_csv(filename.str().c_str(), times, NULL)) {
            exit(EXIT_FAILURE);
        }
        free_vector(times);
    }
}

void args_help(char* prog_name) {
    std::cout << "Quick sort benchmarks for MFarrays, c_vectors and C++ std::vectors" << std::endl;
    std::cout << "Run with : " << prog_name << " <number_of_tests> <number_of_elements> <storage_type>" << std::endl;
    std::cout << "Where:\n" <<
        "  storage_type = 0 = MFarray\n" << 
        "  storage_type = 1 = c_vector\n" <<
        "  storage_type = 2 = C++ std::vector" << std::endl;
    std::cout << "Eg: " << prog_name << " 4 100 1" << std::endl;
}

int main(int argc, char** argv) {
    printf("Starting.\n");

    time_t t;
    srand(time(&t));
    if (argc < 3) {
        quick_sort_tests(TEST_SIZE, REPEATE_COUNT, 0);
    } else {
        int size = 0;
        int repeat = 0;
        int test = 0;
        char* endptr;
        test = strtol(argv[3], &endptr, 10);
        if ((errno != 0 && test == 0) || endptr == argv[3]) {
            perror("Bad args.\n");
            args_help(argv[0]);
            exit(EXIT_FAILURE);
        }
        if(test < 0 || test > 2) {
            perror("Bad args.\n");
            args_help(argv[0]);
            exit(EXIT_FAILURE);
        }
        size = strtol(argv[2], &endptr, 10);
        if ((errno != 0 && size == 0) || endptr == argv[2]) {
            perror("Bad args.\n");
            args_help(argv[0]);
            exit(EXIT_FAILURE);
        }
        repeat = strtol(argv[1], &endptr, 10);
        if ((errno != 0 && size == 0) || endptr == argv[1]) {
            perror("Bad args.\n");
            args_help(argv[0]);
            exit(EXIT_FAILURE);
        }

        printf("Running %i tests on %i items.\n", repeat, size);
        quick_sort_tests(size, repeat, test);
    }
    
    printf("Done.\n");
    return 0;
}


#include <iostream>
#include <deque>
#include <time.h>
#include <unistd.h>
#include <sstream>
#include <random>

#include "common.h"
#include "csv_append.h"
#include "generic_vector.h"
#include "self_reboot.h"

#define TEST_SIZE 4294967296*2
//#define WARMUP 120

int main(int argc, char** argv) {
    std::cout << "Starting.\n" << std::endl;
    std::cout << "process id: " << getpid() << std::endl;
#ifdef WARMUP
    std::cout << "sleeping for "<<WARMUP<<" seconds to let the kernel settle down." << std::endl;
    sleep(WARMUP);
    std::cout << "done sleeping." << std::endl;
#endif

    std::default_random_engine generator;
    //we use the same seed for random indexing into
    //mf_array and c++ dequeue so that its fair
    long int same_seed = 0xDEADBEEFCAFE;
    generator.seed(same_seed);
    std::uniform_int_distribution<long int> distribution(0,TEST_SIZE);

    std::cout << "test size: " << TEST_SIZE << std::endl;

    //label the file with the start time
    struct timespec file_time;
    int res = clock_gettime(CLOCK_REALTIME, &file_time);
    clock_error_check(res);
    std::stringstream filename;
    filename << "c++stddeque_randi_e" << TEST_SIZE << "_t" << (int)file_time.tv_sec << ".csv";

    std::cout << "Filling dequeue" << std::endl;
    std::deque<uint8_t> deq;
    for (long int i = 0; i < TEST_SIZE; i++) {
        deq.push_back(7);
    }

    //start timer
    printf("\nTiming C++ dequeue random access...\n");
    struct timespec start;
    struct timespec end;
    res = clock_gettime(CLOCK_REALTIME, &start);
    clock_error_check(res);

    long int test_max = TEST_SIZE/8;
    for (long int i = 0; i < test_max; i++) {
        if (i == test_max/2) printf("50%% done.\n");
        long int index = distribution(generator);
        uint8_t val = deq.at(index);
        if (val != 7) {
            std::cout << "Error... different value read back: "<<val<<" at index "<<i<<"." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    printf("100%% done.\n");
    save_page_faults(filename.str().c_str());

    //stop timer
    res = clock_gettime(CLOCK_REALTIME, &end);
    clock_error_check(res);
    struct timespec duration = diff(start, end);
    std::cout << "Done, took "<<duration.tv_sec<<" sec, "<<duration.tv_nsec<<" n-sec" << std::endl;

    deq.clear();
    deq.shrink_to_fit();

    //we are only interested in runs that are successful so we save its results after checking
    int temp = 0;
    vector* times = new_vector(3, sizeof(int));
    vec_push_back(times, &temp);
    vec_push_back(times, (temp = duration.tv_sec,  &temp));
    vec_push_back(times, (temp = duration.tv_nsec, &temp));

    if(!append_csv(filename.str().c_str(), times, NULL)) {
        exit(EXIT_FAILURE);
    }
    free_vector(times);

    //std::cout << "Done. Press enter to free memory." << std::endl;
    //std::getc(stdin);
    //std::cout << "Press enter to quit." << std::endl;
    //std::getc(stdin);

    self_reboot();
    std::cout << "Done." << std::endl;

    return EXIT_SUCCESS;
}

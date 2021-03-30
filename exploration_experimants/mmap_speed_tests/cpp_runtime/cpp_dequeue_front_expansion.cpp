
#include <iostream>
#include <deque>
#include <time.h>
#include <unistd.h>

//#define TEST_SIZE 4294967000
  #define TEST_SIZE 2000000000
//#define WARMUP 120

struct timespec diff(struct timespec start, struct timespec end);

int main(int argc, char** argv) {
    std::cout << "Starting.\n" << std::endl;
    std::cout << "process id: " << getpid() << std::endl;
#ifdef WARMUP
    std::cout << "sleeping for "<<WARMUP<<" seconds to let the kernel settle down." << std::endl;
    sleep(WARMUP);
    std::cout << "done sleeping." << std::endl;
#endif
    
    std::cout << "test size: " << TEST_SIZE << std::endl;

    std::deque<uint8_t> deq;

    printf("\nTiming C++ dequeue push_front expansion...\n");
    struct timespec start;
    struct timespec end;
    int res = clock_gettime(CLOCK_REALTIME, &start);
    if (res < 0) std::cout << "Error. with clock_gettime()." << std::endl;

    for (long int i = 0; i < TEST_SIZE; i++) {
        deq.push_front(7);
    }

    res = clock_gettime(CLOCK_REALTIME, &end);
    if (res < 0) std::cout << "Error. with clock_gettime()." << std::endl;
    struct timespec duration = diff(start, end);
    std::cout << "Done, took "<<duration.tv_sec<<" sec, "<<duration.tv_nsec<<" n-sec" << std::endl;

    //double check it actually allocated and wrote..
    std::cout << "Checking that the values("<<deq.size()<<") are valid..." << std::endl;
    for (long int i = 0; i < TEST_SIZE; i++) {
        uint8_t val = deq.at(i);
        if (val != 7) {
            std::cout << "Error... different value read back from first area: "<<val<<" at index "<<i<<"." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    std::cout << "Done. Press enter to free memory." << std::endl;
    std::getc(stdin);
    deq.clear();
    deq.shrink_to_fit();
    std::cout << "Press enter to quit." << std::endl;
    std::getc(stdin);
    return 0;
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

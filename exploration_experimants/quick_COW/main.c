
#include <stdio.h>
#include <unistd.h>

#include "cow_map.h"

int main(int argc, char** argv) {
    printf("Starting.\n");

    cow_map* mp = new_cow_map(sizeof(uint8_t));
    printf("mp info:\n");
    cow_map_info(mp);
    
    for (int i = 0; i < mp->max_size; i++) {
        uint8_t seven = 7;
        cow_map_insert(mp, &seven, i);
    }
    
    cow_map* mp_child1 = duplicate_cow_map(mp);
    printf("mp_child1 info:\n");
    cow_map_info(mp_child1);
    
    for (int i = getpagesize(); i < getpagesize()*2; i++) {
        uint8_t eight = 8;
        cow_map_insert(mp_child1, &eight, i);
    }


    printf("first map:\n");
    for (int i = 0; i < 4; i++) {
        uint8_t* val = cow_map_at(mp, (i*getpagesize())+3);
        printf("\tVal : %i was at addr :%i\n", *val, (i*getpagesize())+3);
    }

    printf("child map 1:\n");
    for (int i = 0; i < 4; i++) {
        uint8_t* val = cow_map_at(mp_child1, (i*getpagesize())+3);
        printf("\tVal : %i was at addr :%i\n", *val, (i*getpagesize())+3);
    }

    printf("Done.\n");
    return 0;
}

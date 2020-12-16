#ifndef PREDICATE_h
#define PREDICATE_h

#include "generic_vector.h"

typedef struct predicate {
    char* name;
    vector* subs;
} predicate;

#endif
#ifndef PREDICATE_STRUCT_H
#define PREDICATE_STRUCT_H

#include <stdbool.h>

#include "generic_vector.h"

enum pred_res {Fail, Pass, Uneval};
typedef struct predicate {
    //If expanded with variables and unification, then the subs should
    //be stored as a list of their names, that way they can match on
    //multiple definitions.
    vector* subs;//predicate* type
    int sub_count;//can now be factored out due to size(vector)
    enum pred_res res;
    char* name;
    bool headless;
} predicate;

#endif
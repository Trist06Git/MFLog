#ifndef PREDICATE_STRUCT_H
#define PREDICATE_STRUCT_H

enum pred_res {Fail, Pass, Uneval};
typedef struct predicate {
    struct predicate* subs;
    int sub_count;
    enum pred_res res;
    char* name;
} predicate;

#endif
#ifndef UTILS_H
#define UTILS_H

#include "internal_struct.h"
#include "generic_vector.h"

static char* unique_prefix = "U_";

function* get_fdef(vector*, char* name);

int digits(int);
int digits_neg(int);
char* unique_name(int*);
char* unique_name_incr(int*);
char* file_to_string(const char*);

#endif
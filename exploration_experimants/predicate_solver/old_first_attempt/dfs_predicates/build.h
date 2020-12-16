#ifndef BUILD_H
#define BUILD_H

#include "generic_vector.h"
#include "parse.h"
#include "predicate_struct.h"

int build_heads(vector* pred_defs, vector* database);
int build_bodies(vector* pred_defs, vector* database);
predicate* get_sub(char* name, vector* database);

#endif
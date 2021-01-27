#ifndef UTILS_H
#define UTILS_H

#include "internal_struct.h"
#include "generic_vector.h"

static char* unique_prefix = "U_";

choice_point* get_cpoint(vector* cps, function*);
bool func_point_exists(vector* cps, function*);
bool func_def_exists(vector* defs, function*);
bool func_arity_exists(vector* defs, fcall*);
bool compare_func_heads(function*, function*);
bool compare_func_arity(function*, function*);
bool compare_func_vals(function*, function*);
function* get_fdef(vector*, char* name);
function* get_fdef_defined(vector*, char* name);
function* get_fdef_arity(vector*, char* name, int arity);

int digits(int);
int digits_neg(int);
char* unique_name(int*);
char* unique_name_incr(int*);
char* file_to_string(const char*);

#endif
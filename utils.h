#ifndef UTILS_H
#define UTILS_H

#include "internal_struct.h"
#include "generic_vector.h"

#define nl printf("\n")

static char* unique_prefix    = "U_";
static char* decompose_prefix = "D_";
static char* tuple_prefix     = "T_";
static char* list_prefix      = "L_";

static char* random_error = "???";

choice_point* get_cpoint(vector* cps, function*);
choice_point* get_cpoint_na(vector* cps, const char* name, int arity);
bool func_point_exists(vector* cps, function*);
bool func_def_exists(vector* defs, function*);
bool func_arity_exists(vector* defs, fcall*);
bool compare_func_heads(function*, function*);
bool compare_func_arity(function*, function*);
bool compare_func_vals(function*, function*);
function* get_fdef(vector*, char* name);
function* get_fdef_defined(vector*, char* name);
function* get_fdef_arity(vector*, char* name, int arity);
int get_cp_count(vector* cps, fcall*);

int digits(int);
int digits_neg(int);
char* unique_name(int*);
char* unique_name_incr(int*);
char* anon_name(const char* prefix, int, int);
char* anon_list_name(const char* prefix, int frm, int lst, int arg);
bool  compare_list_sequ(expr* vr, int sequ);
char* decomp_name(int* arity, int* func);
char* decomp_name_incr(int* arity, int* func);
bool  compare_decomp_sequ(expr* vr, int sequ);
char* file_to_string(const char*);

#endif
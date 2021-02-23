#ifndef EXPLICATION_H
#define EXPLICATION_H

#include "internal_struct.h"
#include "generic_vector.h"
#include "generic_map.h"

extern vector* func_defs_cp;
extern vector* global_defs;

void explic_func(function*, vector* func_defs);
void explic_func_expr(function*, vector* func_defs);
void explic_func_head(function*);
void rec_explic_func_expr(expr*, int* unique, vector* fcs_to_move, vector* singletons, vector* func_defs, function* root);
void rec_explic_func_equ(expr*, int* unique, vector* fc_to_move, vector* singles, vector* func_defs, function* root);
expr tuplise_params(vector* params, vector* singles);
void explic_func_params(fcall*, int* unique, vector* fcs_to_move, vector* singletons, vector* func_defs, function* root);
void explic_callsite(fcall*, int* unique, vector* newly_generated, vector* func_defs);
//I think disimplicate actually means to nolonger imply any connection at all
//explicate should probably be used instead

vector* get_var_singles(function*);///maybe move this to utils or internal_struct
void rec_get_var_singles(expr*, map*);
void check_var_singles_params(vector*, map*);

#endif
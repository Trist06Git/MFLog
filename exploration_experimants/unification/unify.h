#ifndef UNIFY_H
#define UNIFY_H

#include "internal_struct.h"

static char* fail_string = "fail";
static char* pass_string = "pass";
static char* answ_string = "answers";

typedef equality substitution;
typedef enum outcome outcome;
typedef struct frame frame;
typedef struct frame_call frame_call;

enum outcome {o_fail, o_answers, o_pass};

struct frame {
    char* fname;//just for info/debug
    vector* G;//of substitutions
    vector* next_calls;//of frame_calls
    outcome last_result;
};

struct frame_call {
    fcall fc;
    int call_sequence;//so that return vars dont clash
};

void entry(vector* func_defs_cp, vector* globals);
outcome call(frame_call*, frame* prev, vector* func_defs_cp, vector* globals, int* call_sequ);
outcome unify(frame*, vector* func_defs_cp, vector* globals, int* call_sequ);
frame* init_frame(function*, fcall*, vector* globals, int* call_sequ);
void free_frame(frame*);
void add_frame_exprs(frame*, expr*, int* call_sequ);
void rec_add_expr(frame*, expr*, int* call_sequ);//old
void decompose(frame*);
void swap_substitution(substitution*);
void delete_g(frame*);
outcome conflict(frame*);
void eliminate(frame*);
void double_list_eliminate(list* lst1, list* lst2, frame*);
void list_eliminate(list*, const expr* var, const expr* val);
void tuple_eliminate(tuple*, const expr* var, const expr* val);
void rec_tuple_eliminate(and*, const expr* var, const expr* val);
outcome query(expr*);
vector* head_results(frame*, function*);

bool is_list_instantiated_e(expr*);
char* outcome_to_string(const outcome*);

//may not need
void instantiate_a(atom* vr, atom* vl);
void instantiate_e(expr* vr, expr* vl);
void rec_instantiate_e(expr* in, expr* vr, expr* vl);
//end

void dump_frame(frame*);

#endif
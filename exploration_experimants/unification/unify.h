#ifndef UNIFY_H
#define UNIFY_H

#include "internal_struct.h"

static char* fail_string  = "fail";
static char* pass_string  = "pass";
static char* answ_string  = "answers";
static char* undet_string = "undetermined";

typedef equality substitution;
typedef enum outcome outcome;
typedef struct frame frame;
typedef struct frame_call frame_call;

enum outcome {o_fail, o_answers, o_pass, o_undet};

struct frame {
    char* fname;//just for info/debug
    vector* G;//of substitutions
    vector* G_clean;
    vector* next_calls;//of frame_calls
    int call_sequ;//this frames sequence
    outcome last_result;
    bool changes;//have any vars changed? The nondets want to know.
};

struct frame_call {
    fcall fc;
    int call_sequence;//so that return vars dont clash
    int cp_count;//keep track of which cp we attempted last
    bool undet;//has the complete outcome of this fc been completely determined?
};

void entry(vector* func_defs_cp, vector* globals);
outcome call(frame_call*, frame* prev, frame* next, vector* func_defs_cp, vector* globals, int* call_sequ);
outcome unify(frame*, vector* func_defs_cp, vector* globals, int* call_sequ);
frame* init_frame(function*, fcall*, frame* prev_frm, vector* globals, int* global_call_sequ, int this_call_sequ);
void free_frame(frame*);
void add_frame_exprs(frame*, expr*, int* call_sequ);
void rec_add_expr(frame*, expr*, int* call_sequ);//old
void prepend_unique_var_e(expr* vr, int call_sequ);
vector* duplicate_G(vector* G);
void free_G(vector* G);
substitution* get_sub_frm(frame*, expr* var);
substitution* get_sub_frm_i(frame*, int call_no, int var_no);
void decompose(frame*);
void swap_substitution(substitution*);
void delete_g(frame*);
outcome conflict(frame*);
void eliminate(frame*);
void double_list_eliminate(list* lst1, list* lst2, frame*);
void list_eliminate(list*, const expr* var, const expr* val, frame*);
void tuple_eliminate(tuple*, const expr* var, const expr* val, frame*);
//void rec_tuple_eliminate(and*, const expr* var, const expr* val);
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
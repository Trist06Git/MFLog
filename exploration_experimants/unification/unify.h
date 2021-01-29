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
    vector* G;//of substitutions
    vector* next_calls;//of frame_calls
    outcome last_result;
};

struct frame_call {
    fcall fc;
    int call_sequence;//so that return vars dont clash
};

void entry(vector* func_defs_cp);
outcome unify(frame*, vector* func_defs_cp, int* call_sequ);
frame* init_frame(function*, int* call_sequ);
void free_frame(frame*);
void rec_add_expr(frame*, expr*, int* call_sequ);
void swap_substitution(substitution*);
void delete_g(frame*);
void decompose(frame*);
outcome conflict(frame*);
void eliminate(frame*);
outcome query(expr*);

char* outcome_to_string(const outcome*);

//may not need
void instantiate_a(atom* vr, atom* vl);
void instantiate_e(expr* vr, expr* vl);
void rec_instantiate_e(expr* in, expr* vr, expr* vl);
//end

void dump_frame(frame*);

#endif
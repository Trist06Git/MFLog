#ifndef PARSE_H
#define PARSE_H

#include <stdbool.h>

#include "generic_vector.h"

#define TAG_PC 0
#define TAG_AND 1

typedef struct expr expr;
typedef struct pred_call pred_call;
typedef struct and and;
typedef struct pred_def pred_def;

struct expr {
    int tag;
    union {
        pred_call* pc;
        and* a;
    };
};

struct pred_call {
    char* name;
};

struct and {
    expr lhs;
    expr rhs;
};

struct pred_def {
    char* name;
    expr ex;
    bool headless;
};

enum e_errors {er_OK          =  0,
               er_EXPR        = -1,
               er_EXPR_SINGLE = -2,
               er_EXPR_AND    = -3,
               er_PRED_DEF    = -4,
               er_PRED_END    = -5
              };
char* error_to_string(enum e_errors);

int init_parser(bool);
void close_parser(vector* parse_tree, bool);
int get_unique(void);
char* unique_head(void);
enum e_errors parse(vector* tokens, vector* database);
int parse_def(vector* tokens, vector* database);
int parse_expr(vector* tokens, expr*);
void free_expr(expr*);
bool is_match(vector* tokens, vector* pattern);

int get_sub_names(expr*, vector* names, int* count);

/*
expr -> pred_call
expr -> expr AND expr
pred_call -> PRED
pred_def -> PRED NECK expr END_PRED 
thing :- pred1 , pred2 , pred3 .
*/

#endif
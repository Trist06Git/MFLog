#ifndef PARSE_H
#define PARSE_H

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
};

//PRED, NECK, AND, END_PRED, ERROR
/*
expr -> pred_call
expr -> expr AND expr
pred_call -> PRED
pred_def -> PRED NECK expr END_PRED 
thing :- pred1 , pred2 , pred3 .
*/

#endif
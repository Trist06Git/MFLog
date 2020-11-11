
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "lex.h"

vector* p_pred_def;
vector* p_pred_end;

bool is_match(vector* tokens, vector* pattern);

int main(void) {
    printf("Starting.\n");
    
    //vector* tokens = new_vector(0, sizeof(s_token));
    //tokens->type = "s_token";
    //parse_string(tokens, test_prog);
    //dump_tokens(tokens);

    p_pred_def = new_vector(2, sizeof(enum e_token));
    push_back(p_pred_def, PRED);
    push_back(p_pred_def, NECK);
    p_pred_end = new_vector(1, sizeof(enum e_token));
    push_back(p_pred_end, END_PRED);

    //thing :- pred1 , pred2 , pred3 .
    pred_def thing;
    thing.name = "thing";
    thing.ex.a = malloc(sizeof(and));
    thing.ex.tag = TAG_AND;
    thing.ex.a->lhs.pc = malloc(sizeof(pred_call));
    thing.ex.a->lhs.tag = TAG_PC;
    thing.ex.a->lhs.pc->name = "pred1";
    thing.ex.a->rhs.a = malloc(sizeof(and));
    thing.ex.a->rhs.tag = TAG_AND;
    thing.ex.a->rhs.a->lhs.pc = malloc(sizeof(pred_call));
    thing.ex.a->rhs.a->lhs.tag = TAG_PC;
    thing.ex.a->rhs.a->lhs.pc->name = "pred2";
    thing.ex.a->rhs.a->rhs.pc = malloc(sizeof(pred_call));
    thing.ex.a->rhs.a->rhs.tag = TAG_PC;
    thing.ex.a->rhs.a->rhs.pc->name = "pred3";

    printf("Done.\n");
    return EXIT_SUCCESS;
}

int parse(vector* tokens, vector* database) {
    if (size(tokens) <= 0) return 0; 
    if (is_match(tokens, p_pred_def)) {
        int res = parse_def(tokens, database);
        if (res == -1) return -1;
    }
    if (!is_match(tokens, p_pred_end)) return -1;
    if (size(tokens) > 0) {
        parse(tokens, database);
    } else {
        return 0;
    }
    
}

int parse_def(vector* tokens, vector* database) {
    
    
}

bool is_match(vector* tokens, vector* pattern) {
    for (int i = 0; i < size(tokens) && i < size(pattern); i++) {
        if (((s_token*)at(tokens, i))->tag != *(enum e_token*)at(pattern, i)) return false;
    }
    return true;
}

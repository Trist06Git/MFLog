
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "lex.h"

static int unique;

//patterns to match against
vector* p_pred_def;
vector* p_pred_end;
vector* p_expr_and;
vector* p_expr_call;
vector* p_headless;

int init_parser(bool verbose) {
    if (verbose) printf("Initialising Parser.\n");
    unique = 0;

    enum e_token e_pred = PRED;
    enum e_token e_neck = NECK;
    enum e_token e_end  = END_PRED;
    enum e_token e_and  = AND;

    //PRED, NECK
    p_pred_def = new_vector(2, sizeof(enum e_token));
    push_back(p_pred_def, &e_pred);
    push_back(p_pred_def, &e_neck);

    //END_PRED
    p_pred_end = new_vector(1, sizeof(enum e_token));
    push_back(p_pred_end, &e_end);

    //PRED, AND
    p_expr_and = new_vector(3, sizeof(enum e_token));
    push_back(p_expr_and, &e_pred);
    push_back(p_expr_and, &e_and);
    
    //PRED
    p_expr_call = new_vector(1, sizeof(enum e_token));
    push_back(p_expr_call, &e_pred);

    //NECK, for headless
    p_headless = new_vector(1, sizeof(enum e_token));
    push_back(p_headless, &e_neck);

    return 0;
}

void close_parser(vector* parse_tree, bool verbose) {
    if (verbose) printf("Closing Parser.\n");
    free_vector(p_pred_def);
    free_vector(p_pred_end);
    free_vector(p_expr_and);
    free_vector(p_expr_call);
    free_vector(p_headless);
    /////THIS NEEDS FINISHING..
    //causes segfault
    for (int i = 0; i < size(parse_tree); i++) {
        //free_expr(&((pred_def*)at(parse_tree, i))->ex);
        //free_vector(parse_tree);
    }
}

int get_unique(void) {
    return unique++;
}

char* unique_head(void) {
    int num = get_unique();
    int chars = 0;
    for (int n = num; n; n/=10, chars++);
    //#HEADLESS#+n+/0
    char* head_name = malloc(sizeof(char)*(10+chars+1));
    sprintf(head_name, "#HEADLESS#%i", num);
    
    return head_name;
}

enum e_errors parse(vector* tokens, vector* database) {
    if (size(tokens) <= 0) return 0; 
    if (is_match(tokens, p_pred_def)) {
        enum e_errors res = parse_def(tokens, database);
        if (res != er_OK) return res;
    } else if (is_match(tokens, p_headless)) {
        s_token headless_pred = {.tag = PRED, .name = unique_head()};
        insert_at(tokens, 0, &headless_pred);
        enum e_errors res = parse_def(tokens, database);
        if (res != er_OK) return res;
    }
    if (size(tokens) > 0) {
        enum e_errors res = parse(tokens, database);
        if (res != er_OK) return res;
    } else {
        return er_OK;
    }
}

int parse_def(vector* tokens, vector* database) {
    pred_def new_pred;
    new_pred.name = ((s_token*)at(tokens, 0))->name;//head
    
    remove_at(tokens, 0);//remove head
    remove_at(tokens, 0);//remove neck
    
    enum e_errors res = parse_expr(tokens, &new_pred.ex);
    if (res != er_OK) return res;
    push_back(database, &new_pred);
    if (!is_match(tokens, p_pred_end)) {
        return er_PRED_END;
    } else {
        remove_at(tokens, 0);
    }
    return er_OK;
}

int parse_expr(vector* tokens, expr* ex) {
    if (is_match(tokens, p_expr_and)) {
        ex->tag = TAG_AND;
        ex->a = malloc(sizeof(and));
        ex->a->lhs.tag = TAG_PC;
        ex->a->lhs.pc = malloc(sizeof(pred_call));
        ex->a->lhs.pc->name = ((s_token*)at(tokens, 0))->name;
        remove_at(tokens, 0);//remove call
        remove_at(tokens, 0);//remove and
        enum e_errors res = parse_expr(tokens, &(ex->a->rhs));
        if (res != er_OK) return res;
    } else if (is_match(tokens, p_expr_call)) {
        ex->tag = TAG_PC;
        ex->pc = malloc(sizeof(pred_call));
        ex->pc->name = ((s_token*)at(tokens, 0))->name;
        remove_at(tokens, 0);
    } else {
        return er_EXPR;
    }
    return er_OK;
}

void free_expr(expr* ex) {
    if (ex->tag == TAG_AND) {
        free_expr(&ex->a->lhs);
        free_expr(&ex->a->rhs);
        free(ex->a);
    } else {//TAG_PC
        free(ex->pc);
    }
    free(ex);
}

bool is_match(vector* tokens, vector* pattern) {
    for (int i = 0; i < size(tokens) && i < size(pattern); i++) {
        if (((s_token*)at(tokens, i))->tag != *(enum e_token*)at(pattern, i)) return false;
    }
    return true;
}

int get_sub_names(expr* ex, vector* names, int* count) {
    if (ex->tag == TAG_PC) {
        push_back(names, ex->pc->name);
        (*count)++;
    } else if (ex->tag == TAG_AND) {
        int res;
        res  = get_sub_names(&(ex->a->lhs), names, count);
        res += get_sub_names(&(ex->a->rhs), names, count);
        if (res != 0) return -1;
    } else {
        return -1;
    }
    return 0;
}

char* error_to_string(enum e_errors err) {
    switch (err) {
        case er_OK          : { return "OK";                break; }
        case er_EXPR        : { return "EXPR error";        break; }
        case er_EXPR_SINGLE : { return "EXPR_SINGLE error"; break; }
        case er_EXPR_AND    : { return "EXPR_AND error";    break; }
        case er_PRED_DEF    : { return "PRED_DEF error";    break; }
        case er_PRED_END    : { return "PRED_END error";    break; }
        default             : { return "?";                 break; }
    }
}

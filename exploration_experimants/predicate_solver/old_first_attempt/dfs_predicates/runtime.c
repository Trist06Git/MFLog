
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "runtime.h"
#include "predicate_struct.h"
#include "generic_vector.h"
#include "parse.h"
#include "lex.h"
#include "build.h"

#define nl printf("\n")

int init_runtime(bool verbose) {
    if (verbose) printf("Initialising Runtime.\n");

    predicate taut;
    taut.subs      = NULL;
    taut.sub_count = 0;
    taut.res       = Pass;
    taut.name      = "true";
    taut.headless  = false;
    predicate fail;
    fail.subs      = NULL;
    fail.sub_count = 0;
    fail.res       = Fail;
    fail.name      = "false";
    fail.headless  = false;
    
    database = new_vector(0, sizeof(predicate));
    push_back(database, &taut);
    push_back(database, &fail);
    
    return 0;
}

int close_runtime(bool verbose) {
    if (verbose) printf("Closing up Runtime.\n");
    for (int i = 0; i < size(database); i++) {
        free_vector(((predicate*)at(database, i))->subs);
    }
    free_vector(database);
}

void run(vector* database) {
    for (int i = 0; i < size(database); i++) {
        predicate* pred = at(database, i);
        if (is_headless(pred)) {
            eval_pred(pred);
            print_result(pred);
        }
    }
}

bool is_headless(predicate* pred) {
    char* target = "#HEADLESS#";
    bool res = true;
    for (int i = 0; i < strlen(target); i++) {
        if (i >= strlen(pred->name)) return false;
        res &= target[i] == pred->name[i];
    }
    return res;
}

predicate* pred_head(char* name) {
    predicate* pred = malloc(sizeof(predicate));
    pred->name = name;
    pred->subs = NULL;
    pred->sub_count = 0;
    pred->res = Uneval;
    return pred;
}

void pred_subs(predicate* super, int count, ...) {
    super->sub_count = count;

    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++) {
       predicate* pred = va_arg(args, predicate*);
       push_back(super->subs, &pred);
    }
    va_end(args);
}

void eval_pred(predicate* pred) {
    if (pred->res != Uneval) {
        return;
    } else {
        pred->res = Pass;
        for (int i = 0; i < pred->sub_count; i++) {
            eval_pred((predicate*)at(pred->subs, i));
            if (((predicate*)at(pred->subs, i))->res == Fail) {
                pred->res = Fail;
                return;
            }
        }
    }
}

void print_pred(predicate* pred) {
    printf("Predicate : %s\n", pred->name);
    if (pred->res == Uneval) {
        printf("Is not yet evaluated\n");
    } else if (pred->res == Pass) {
        printf("Has passed\n");
    } else if (pred->res == Fail) {
        printf("Has failed\n");
    } else {
        printf("ERROR..\n");
    }
}

void print_result(predicate* pred) {
    if (pred->res == Uneval) {
        printf(":- ?\n");
    } else if (pred->res == Pass) {
        printf(":- true\n");
    } else if (pred->res == Fail) {
        printf(":- false\n");
    } else {
        printf(":- ERROR..\n");
    }
}

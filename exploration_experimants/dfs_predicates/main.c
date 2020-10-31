
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define nl printf("\n")

enum pred_res {Fail, Pass, Uneval};
typedef struct predicate {
    struct predicate* subs;
    int sub_count;
    enum pred_res res;
    char* name;
} predicate;

predicate taut = {.subs = NULL, .sub_count = 0, .res = Pass, .name = "true"};
predicate fail = {.subs = NULL, .sub_count = 0, .res = Fail, .name = "false"};

predicate database[] = {
    {.subs = &taut, .sub_count = 1, .res = Uneval, .name = "P1"},
    {.subs = &fail, .sub_count = 1, .res = Uneval, .name = "P2"},
    {.subs = NULL, .sub_count = 0, .res = Uneval, .name = "P3"}
};

predicate* pred_head(char*);
void pred_subs(predicate*, int, ...);
void eval_pred(predicate*);
void print_pred(predicate*);

int main(void) {
    printf("Starting.\n");

    /*
    eval_pred(database+0);
    nl;
    print_pred(database+0);
    nl;

    eval_pred(database+1);
    nl;
    print_pred(database+1);
    nl;
    */

    pred_subs(database+2, 4, taut, taut, taut, database[0]);
    eval_pred(database+2);
    nl;
    print_pred(database+2);
    nl;
    
    printf("Done.\n");
    return EXIT_SUCCESS;
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
    super->subs = malloc(sizeof(predicate)*count);

    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++) {
       super->subs[i] = va_arg(args, predicate);
    }
    va_end(args);
}

void eval_pred(predicate* pred) {
    //printf("evaluating %s\n", pred->name);
    print_pred(pred);
    if (pred->res != Uneval) {
        return;
    } else {
        pred->res = Pass;
        for (int i = 0; i < pred->sub_count; i++) {
            eval_pred(pred->subs+i);
            if ((pred->subs+i)->res == Fail) {
                pred->res = Fail;
                return;
            }
        }
    }
}

void print_pred(predicate* pred) {
    printf("Predicate : %s\n", pred->name);
    if (pred->res == Uneval) printf("Is not yet evaluated\n");
    if (pred->res == Pass)   printf("Has passed\n");
    if (pred->res == Fail)   printf("Has failed\n");
}

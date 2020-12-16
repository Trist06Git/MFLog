#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>

#include "predicate_struct.h"
#include "generic_vector.h"

vector* database;

int init_runtime(bool);
int close_runtime(bool);
predicate* pred_head(char*);
void pred_subs(predicate*, int, ...);
void eval_pred(predicate*);
void print_pred(predicate*);
void print_result(predicate*);
void run(vector* database);
bool is_headless(predicate*);

#endif
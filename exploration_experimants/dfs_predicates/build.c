
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "build.h"
#include "predicate_struct.h"

int build_heads(vector* pred_defs, vector* database) {
    for (int i = 0; i < size(pred_defs); i++) {
        predicate pred;
        pred.subs = new_vector(0, sizeof(predicate*));
        pred.sub_count = 0;
        pred.res = Uneval;
        pred.headless = ((pred_def*)at(pred_defs, i))->headless;
        //memcpy this 
        pred.name = ((pred_def*)at(pred_defs, i))->name;
        push_back(database, &pred);
    }
}

int build_bodies(vector* pred_defs, vector* database) {
    for (int i = 0; i < size(pred_defs); i++) {
        vector* names = new_vector(0, sizeof(char*));
        int count = 0;
        pred_def* this_pred = (pred_def*)at(pred_defs, i);
        get_sub_names(&this_pred->ex, names, &count);
        for (int j = 0; j < size(database); j++) {
            predicate* that_pred = at(database, j);
            if (strcmp(this_pred->name, that_pred->name) == 0) {
                for (int k = 0; k < count; k++) {
                    predicate* this_sub = get_sub((char*)at(names, k), database);
                    push_back(that_pred->subs, &this_sub);
                    that_pred->sub_count++;
                }
                break;
            }
        }
        free_vector(names);
    }
    return 0;
}

//needs error handling..
predicate* get_sub(char* name, vector* database) {
    for (int i = 0; i < size(database); i++) {
        predicate* pred = at(database, i);
        if (strcmp(pred->name, name) == 0) {
            return pred;
        }
    }
    return get_sub("false", database);
}


#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "internal_struct.h"
#include "generic_vector.h"

and append_exprs_and(vector* exprs) {
    and this;
    this.lhs = malloc(sizeof(expr));
    this.rhs = malloc(sizeof(expr));
    
    memcpy(this.lhs, at(exprs, 0), sizeof(expr));
    remove_at(exprs, 0);
    if (size(exprs) == 1) {
        memcpy(this.rhs, at(exprs, 0), sizeof(expr));
    } else {
        expr next;
        next.type = e_and;
        next.e.n = append_exprs_and(exprs);
        memcpy(this.rhs, &next, sizeof(expr));
    }
    return this;
}

and append_exprs_and_init(vector* exprs) {
    if (size(exprs) < 2) {
        and res = {NULL, NULL};//really not good.. oh well.
        return res;
    } else {
        return append_exprs_and(exprs);
    }
}


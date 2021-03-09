
#include <string.h>

#include "builtin_calls.h"
#include "utils.h"
#include "debug_stuff.h"

substitution make_uni_sub(int call_sequ, int param_sequ) {
    substitution s;
    s.lhs = malloc(sizeof(expr));
   *s.lhs = make_var_e(decomp_name(&call_sequ, &param_sequ));
    s.rhs = malloc(sizeof(expr));
    return s;
}

outcome integer_builtin(fcall* fc, frame* frm, int call_sequ) {
    if (vec_size(fc->params) != 1) {
        printf("Error. Could not find function \"%s\" with arity %i\n", fc->name, vec_size(fc->params));
        return o_fail;
    }
    expr* param = vec_at(fc->params, 0);
    if (is_var_e(param)) {
        if (fc->res_set == rs_first || fc->res_set == rs_one || fc->res_set == rs_n) {
            substitution s = make_uni_sub(call_sequ, 0);
            *s.rhs = make_int_e(0);
            vec_push_back(frm->G, &s);
            return o_pass;
        } else if (fc->res_set == rs_n) {//no way of communicating this in the parse yet.
            substitution s = make_uni_sub(call_sequ, fc->rs_index);//isnt it func/arity???
            *s.rhs = make_int_e(0);
            vec_push_back(frm->G, &s);
            return o_pass;
        } else if (fc->res_set == rs_all) {
            printf("Error. Failed to return all the integers...\n");
            return o_fail;
        } else {
            printf("Internal :: Unknown result set quantifier.\n");
            return o_fail;
        }
    } else if (is_val_e(param) && param->e.a.data.vl.type == v_int) {
        if (fc->res_set == rs_first && param->e.a.data.vl.v.i != 0) return o_fail;
        return o_pass;
    } else {
        return o_fail;
    }
}

outcome ltgt_builtin(fcall* fc, frame* frm, int call_sequ) {
    int zero = 0;
    int one  = 1;
    expr param1 = make_var_e(decomp_name(&call_sequ, &zero));
    expr param2 = make_var_e(decomp_name(&call_sequ, &one));
    substitution* s1 = get_sub_frm(frm, &param1);
    substitution* s2 = get_sub_frm(frm, &param2);
    free(param1.e.a.data.vr.symbol);
    free(param2.e.a.data.vr.symbol);
    if (s1 == NULL || s2 == NULL) {
        printf("Error. \"<\" & \">\" need 2 operands.\n");
        return o_fail;
    }
    expr* lhs = s1->rhs;
    expr* rhs = s2->rhs;
    if (strcmp(fc->name, "greater_than") == 0) {
        expr* temp = lhs;
        lhs = rhs;
        rhs = temp;
    }
    if (fc->res_set == rs_one || fc->res_set == rs_first) {
        if (is_list_e(lhs)) {
            lhs = mfa_at(lhs->e.a.data.vl.v.l.lst, 0);//check empty list
        }
        if (is_list_e(rhs)) {
            rhs = mfa_at(rhs->e.a.data.vl.v.l.lst, 0);
        }
        if (is_var_e(lhs) && is_var_e(rhs)) {
            printf("Note. Could not find all the integers that are less than all the other integers.\n");
            return o_undet;
        } else if (is_val_e(lhs) && is_val_e(rhs)) {
            if (lhs->e.a.data.vl.v.i < rhs->e.a.data.vl.v.i)
                return o_pass;
            else
                return o_fail;
        } else if (is_var_e(lhs)) {
            return o_undet;
            // substitution s = make_uni_sub(call_sequ, 0);
            //*s.rhs = make_int_e(rhs->e.a.data.vl.v.i - 1);
            // vec_push_back(frm->G, &s);
            // return o_pass;
        } else if (is_var_e(rhs)) {
            return o_undet;
            // substitution s = make_uni_sub(call_sequ, 1);
            //*s.rhs = make_int_e(lhs->e.a.data.vl.v.i + 1);
            // vec_push_back(frm->G, &s);
            // return o_pass;
        } else {
            printf("Internal :: ltgt_builtin() returned abnormally.\n");
            return o_fail;
        }
    } else {
        printf("Info. Not all answer sets quantifiers have been implemented for \"<\" & \">\" yet sorry.\n");
        return o_fail;
    }
}

outcome print_builtin(fcall* fc, frame* frm, int call_sequ) {
    int zero = 0;
    expr param = make_var_e(decomp_name(&call_sequ, &zero));
    substitution* s = get_sub_frm(frm, &param);
    free(param.e.a.data.vr.symbol);
    if (s == NULL) {
        printf("Error. \"print\" needs a parameter.\n");
        return o_fail;
    }

    expr* subject = s->rhs;
    if (is_var_e(subject)) {
        return o_undet;
    } else if (is_list_e(subject) && !is_list_instantiated_e(subject)) {
        return o_undet;
    } else if (is_val_e(subject)) {
#ifdef UNIFY_DEBUG
        printf("DEBUG :: output :\n");
#endif
        if (!is_list_e(subject)) {
            printf("%i", subject->e.a.data.vl.v.i);
        } else {
            dump_list(&subject->e.a.data.vl.v.l);
        }
        return o_pass;
    } else {
        printf("Internal :: print_builtin() encountered an unknown expression.\n");
        return o_fail;
    }
}

outcome nl_builtin(fcall* fc, frame* frm, int call_sequ) {
    for (int i = 0; ; i++) {
        expr parami = make_var_e(decomp_name(&call_sequ, &i));
        substitution* s = get_sub_frm(frm, &parami);
        free(parami.e.a.data.vr.symbol);
        if (i == 0 && s == NULL) {
            printf("\n");
            return o_pass;
        } else if (s == NULL) {
            break;
        } else if (is_var_e(s->rhs)) {
            return o_undet;
        }
    }
    printf("\n");
    return o_pass;
}

outcome plus_builtin(fcall* fc, frame* frm, int call_sequ) {
    if (vec_size(fc->params) != 3) {
        printf("Error. \"+/plus\" requires 3 operands.\n");
        return o_fail;
    }
    
    substitution* sX = get_sub_frm_i(frm, call_sequ, 0);
    substitution* sY = get_sub_frm_i(frm, call_sequ, 1);
    substitution* sR = get_sub_frm_i(frm, call_sequ, 2);
    expr* X = sX == NULL || is_var_e(sX->rhs) ? NULL : sX->rhs;
    expr* Y = sY == NULL || is_var_e(sY->rhs) ? NULL : sY->rhs;
    expr* R = sR == NULL || is_var_e(sR->rhs) ? NULL : sR->rhs;
    if (X != NULL && Y != NULL && R != NULL) {
        //check
        if (is_int_e(X) && is_int_e(Y) && is_int_e(R)) {
            if (X->e.a.data.vl.v.i + Y->e.a.data.vl.v.i == R->e.a.data.vl.v.i) {
#ifdef UNIFY_DEBIG
                printf("DEBUG :: plus : checking:\n");
                printf("%i + %i == %i ; ans:%i\n",
                    X->e.a.data.vl.v.i,
                    Y->e.a.data.vl.v.i,
                    R->e.a.data.vl.v.i,
                    X->e.a.data.vl.v.i + Y->e.a.data.vl.v.i
                );
#endif
                return o_pass;
            }
        }
#ifdef UNIFY_DEBUG
        printf("DEBUG :: plus : returned as a failure\n");
#endif
        return o_fail;

    } else if (X != NULL && Y != NULL && R == NULL) {
        ///regular x + y = r
        //result var
        substitution s1 = make_uni_sub(call_sequ, 2);
       *s1.rhs = make_int_e(X->e.a.data.vl.v.i + Y->e.a.data.vl.v.i);
        vec_push_back(frm->G, &s1);
        //passed var
        substitution s2 = make_uni_sub(call_sequ, 2);
       *s2.rhs = *s2.lhs;
       *s2.lhs = copy_expr(vec_at(fc->params, 2));
        vec_push_back(frm->G, &s2);
        
        return o_pass;

    } else if (X != NULL && Y == NULL && R != NULL) {
        ///subtraction y = r - x
        //result var
        substitution s = make_uni_sub(call_sequ, 1);
       *s.rhs = make_int_e(R->e.a.data.vl.v.i - X->e.a.data.vl.v.i);
        vec_push_back(frm->G, &s);
        //passed var
        substitution s2 = make_uni_sub(call_sequ, 1);
       *s2.rhs = *s2.lhs;
       *s2.lhs = copy_expr(vec_at(fc->params, 1));
        vec_push_back(frm->G, &s2);

        return o_pass;
    } else if (X == NULL && Y != NULL && R != NULL) {
        ///subtraction x = r - y
        //result var 
        substitution s = make_uni_sub(call_sequ, 0);
       *s.rhs = make_int_e(R->e.a.data.vl.v.i - Y->e.a.data.vl.v.i);
        vec_push_back(frm->G, &s);
        //passed var
        substitution s2 = make_uni_sub(call_sequ, 0);
       *s2.rhs = *s2.lhs;
       *s2.lhs = copy_expr(vec_at(fc->params, 0));
        vec_push_back(frm->G, &s2);

        return o_pass;
    } else {
#ifdef UNIFY_DEBIG
        printf("Info. Sorry that permutation of \"+\" operands has not yet been implemented.\n");
        printf("Namely:\n");
        printf("X = "); if (X==NULL) {printf("NULL");} else {dump_expr(*X, false);} nl;
        printf("Y = "); if (Y==NULL) {printf("NULL");} else {dump_expr(*Y, false);} nl;
        printf("R = "); if (R==NULL) {printf("NULL");} else {dump_expr(*R, false);} nl;
#endif
        return o_undet;
    }

#ifdef UNIFY_DEBUG
        printf("DEBUG :: plus : returned as a failure\n");
#endif
    return o_fail;
}

outcome call_builtin(fcall* fc, frame* frm, int call_sequ) {
    if (strcmp(fc->name, "integer") == 0) {
        return integer_builtin(fc, frm, call_sequ);

    } else if (strcmp(fc->name, "less_than") == 0 ||
               strcmp(fc->name, "greater_than") == 0) {
        return ltgt_builtin(fc, frm, call_sequ);

    } else if (strcmp(fc->name, "print") == 0) {
        return print_builtin(fc, frm, call_sequ);

    } else if (strcmp(fc->name, "nl") == 0) {
        return nl_builtin(fc, frm, call_sequ);

    } else if (strcmp(fc->name, "plus") == 0) {
        return plus_builtin(fc, frm, call_sequ);

    } else {
        printf("Info. Sorry, builtin function \"%s\" is not yet implemented.\n", fc->name);
        return o_fail;
    }
}

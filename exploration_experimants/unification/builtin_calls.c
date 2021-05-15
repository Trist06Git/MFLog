
#include <string.h>
#include <stdbool.h>

#include "builtin_calls.h"
#include "internal_struct.h"
#include "utils.h"
#include "debug_stuff.h"

outcome integer_builtin(fcall* fc, frame* frm, int call_sequ) {
    if (vec_size(fc->params) != 1) {
        printf("Error. Could not find function \"%s\" with arity %li\n", fc->name, vec_size(fc->params));
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
    expr param1 = make_var_e(make_decomp_s(call_sequ, zero));
    expr param2 = make_var_e(make_decomp_s(call_sequ, one));
    substitution* s1 = get_sub_frm(frm, &param1);
    substitution* s2 = get_sub_frm(frm, &param2);
    free_expr(&param1);
    free_expr(&param2);
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
    expr param = make_var_e(make_decomp_s(call_sequ, zero));
    substitution* s = get_sub_frm(frm, &param);
    free_expr(&param);
    if (s == NULL) {
        printf("Error. \"print-%i\" needs a parameter.\n", call_sequ);
        return o_fail;
    }

    expr* subject = s->rhs;
    if (is_var_e(subject)) {
        //return o_undet;
        return o_pass;
    } else if (is_list_e(subject) && !is_list_instantiated_e(subject)) {
        //return o_undet;
        return o_pass;
    } else if (is_val_e(subject)) {
#ifdef UNIFY_DEBUG
        printf("DEBUG :: output :\n");
#endif
        if (!is_list_e(subject)) {
            printf("%i", subject->e.a.data.vl.v.i);
        } else {
            dump_list(&subject->e.a.data.vl.v.l, false);
        }
        return o_pass;
    } else {
        printf("Internal :: print_builtin() encountered an unknown expression.\n");
        return o_fail;
    }
}

outcome nl_builtin(fcall* fc, frame* frm, int call_sequ) {
    for (int i = 0; ; i++) {
        expr parami = make_var_e(make_decomp_s(call_sequ, i));
        substitution* s = get_sub_frm(frm, &parami);
        free_expr(&parami);
        if (i == 0 && s == NULL) {
            printf("\n");
            return o_pass;
        } else if (s == NULL) {
            break;
        } else if (is_var_e(s->rhs)) {
            //return o_undet;
            return o_pass;
        }
    }
    printf("\n");
    return o_pass;
}

expr wrap_mfa_e(mf_array* lst, enum v_type type, bool has_vars) {
    expr ret = {
        .type = e_atom,
        .e.a = {
            .type = a_val,
            .data.vl = {
                .type = v_list, .v.l = {
                    .type = type,
                    .has_vars = has_vars,
                    .lst = lst
                }
            }
        }
    };
    return ret;
}

outcome add_to_list(list* lst, expr* ex) {
    if (lst->type == v_notype && is_val_e(ex)) {
        lst->type = ex->e.a.data.vl.type;
    } else if (is_val_e(ex) && ex->e.a.data.vl.type != lst->type) {
        return o_fail;
    }
        
    if (lst->lst == NULL) {
        lst->lst = new_mfarray(sizeof(expr));
    }
    
    if (is_var_e(ex)) {
        lst->has_vars = true;
    }
    expr new_ex = copy_expr(ex);
    mfa_push_front(lst->lst, &new_ex);
    return o_pass;
}

outcome at_index_builtin(fcall* fc, frame* frm, int call_sequ) {
    if (vec_size(fc->params) < 3) {
        printf("Error. \"[]/at_index\" requires 3+ operands.\n");
        return o_fail;
    }
    substitution* L = get_sub_frm_i(frm, call_sequ, 0);
    substitution* I = get_sub_frm_i(frm, call_sequ, 1);
    substitution* R = get_sub_frm_i(frm, call_sequ, 2);
    if (is_var_e(L->rhs)) {
        printf("Error. :: List cannot be variable.\n");
        return o_fail;
    } else if (is_var_e(I->rhs)) {
        printf("Error. :: Index query not yet implemented.");
        return o_fail;
    } else if (is_list_e(L->rhs) && is_int_e(I->rhs)) {//list is val, index is val, result is val/var
        int index = I->rhs->e.a.data.vl.v.i;
        mf_array* arr = L->rhs->e.a.data.vl.v.l.lst;
        substitution new_R = make_uni_sub(call_sequ, 2);
        expr* ex = mfa_at(arr, index);
        if (ex == NULL) {
            #ifdef UNIFY_DEBUG
            printf("ERROR :: []/at_index : Bad index requested at %i in ", index);dump_expr(*L->rhs, false);nl;
            #endif
            return o_fail;//not within bounds
        }
       *new_R.rhs = copy_expr(ex);
        vec_push_back(frm->G, &new_R);
        return o_pass;
    }

    return o_fail;
}

outcome at_set_builtin(fcall* fc, frame* frm, int call_sequ) {
    if (vec_size(fc->params) < 3) {
        printf("Error. \":=/at_set\" requires 3+ operands.\n");
        return o_fail;
    }
    substitution* L = get_sub_frm_i(frm, call_sequ, 0);
    substitution* I = get_sub_frm_i(frm, call_sequ, 1);
    substitution* E = get_sub_frm_i(frm, call_sequ, 2);
    if (is_var_e(L->rhs) || is_var_e(I->rhs)) {
        printf("Error :: List or Index cannot be variable.\n");
        return o_fail;
    } else if (E == NULL) {
        printf("Internal :: Element for at_set() is NULL.\n");
    } else if (is_int_e(I->rhs) && is_list_e(L->rhs)) {
        int index = I->rhs->e.a.data.vl.v.i;
        mf_array* arr = L->rhs->e.a.data.vl.v.l.lst;
        expr* old_el = mfa_at(arr, index);
        if (old_el != NULL) {//NULL may mean push front/back
            free_expr(old_el);
        }
        expr new_el = copy_expr(E->rhs);
        if (mfa_set(arr, index, &new_el) == false) {
            //bad index
            free_expr(&new_el);
            return o_fail;
            #ifdef UNIFY_DEBUG
            printf("ERROR :: :=/at_set : Bad index requested at %i in ", index);dump_expr(*L->rhs, false);nl;
            #endif
        } else {
            return o_pass;
        }
    }

    return o_fail;
}

outcome card_builtin(fcall* fc, frame* frm, int call_sequ) {
    if (vec_size(fc->params) < 2) {//maybe no point to this
        printf("Error. \"card\" requires 2+ operands.\n");
        return o_fail;
    }
    substitution* L = get_sub_frm_i(frm, call_sequ, 0);
    substitution* R = get_sub_frm_i(frm, call_sequ, 1);
    expr* P1 = vec_at(fc->params, 0);
    if (L != NULL && is_list_e(L->rhs) && R != NULL && is_int_e(R->rhs)) {
        list* lst = &L->rhs->e.a.data.vl.v.l;
        int card = R->rhs->e.a.data.vl.v.i;
        if (lst->lst != NULL && mfa_card(lst->lst) == card) {
            return o_pass;
        } else if (lst->lst == NULL && card == 0) {
            return o_pass;
        } else {
            return o_fail;
        }
    } else if (L != NULL && is_list_e(L->rhs)) {
        list* lst = &L->rhs->e.a.data.vl.v.l;
        substitution new_R = make_uni_sub(call_sequ, 1);
       *new_R.rhs = make_int_e(mfa_card(lst->lst));
        vec_push_back(frm->G, &new_R);
        return o_pass;
    } else if ((L == NULL || (L != NULL && is_var_e(L->rhs))) && (R != NULL && is_var_e(R->rhs))) {
        printf("Error :: card : Many there are many lists with the cardinality %i\n", R->rhs->e.a.data.vl.v.i);
    }
    return o_fail;
}

outcome ref_builtin(fcall* fc, frame* frm, int call_sequ) {
    if (vec_size(fc->params) < 2) {//maybe no point to this
        printf("Error. \"::/cons\" requires 2+ operands.\n");
        return o_fail;
    }
    substitution* L = get_sub_frm_i(frm, call_sequ, 0);
    substitution* R = get_sub_frm_i(frm, call_sequ, 1);
    expr* P1 = vec_at(fc->params, 0);
    if (is_var_e(L->rhs)) {
        //should be undet, then put on the backburner and retried once vars
        //are instantiated.
        return o_fail;
    } else if (is_list_e(L->rhs)) {
        substitution new_R = make_uni_sub(call_sequ, 1);
       *new_R.rhs = reference_list(&L->rhs->e.a.data.vl.v.l);
        vec_push_back(frm->G, &new_R);
        return o_pass;
    } else if (is_list_e(P1)) {
        substitution new_R = make_uni_sub(call_sequ, 1);
       *new_R.rhs = reference_list(&P1->e.a.data.vl.v.l);
        vec_push_back(frm->G, &new_R);
        return o_pass;
    } else {
        //not list/other fails
        return o_fail;
    }

}

//the 2nd last param of fc is always the list to cons to.
//the first n params of fc are the elements to cons.
//last param is the return var
outcome cons_builtin(fcall* fc, frame* frm, int call_sequ) {
    if (vec_size(fc->params) < 3) {
        printf("Error. \"::/cons\" requires 3+ operands.\n");
        return o_fail;
    }
    //Next: also: _ gets removed before call, so D_x_1 doesnt exist, resulting in NULL
    //R for Return, L for List
    substitution* R = get_sub_frm_i(frm, call_sequ, vec_size(fc->params)-1);
    substitution* L = get_sub_frm_i(frm, call_sequ, vec_size(fc->params)-2);
    //expr* ret = get_sub_frm_i(frm, call_sequ, vec_size(fc->params)-1)->rhs;
    //expr* lst = get_sub_frm_i(frm, call_sequ, vec_size(fc->params)-2)->rhs;
    
    if (is_wild_e(L->rhs) && is_wild_e(R->rhs)) {
        return o_pass;
    } else if (is_var_e(L->rhs) && is_var_e(R->rhs)) {
        printf("Error. \"::/cons\" was givven result and list as vars\n");
        return o_fail;
    } else if (is_var_e(L->rhs)) {
        //case for find tail, and confirm head
        //add subs for first n cons vars
        //substitution* L = get_sub_frm_i(frm, call_sequ, vec_size(fc->params)-2);
        list* l_ret = &R->rhs->e.a.data.vl.v.l;
        if (l_ret->lst == NULL) return o_fail;//anything cons'ed cannot = []
        int start = vec_size(fc->params)-2;
        for (int i = 0; i < start && i < mfa_card(l_ret->lst); i++) {
            substitution* param = get_sub_frm_i(frm, call_sequ, i);
            if (param == NULL) continue;
            substitution s = alloc_sub();
           *s.lhs = copy_expr(param->lhs);
           *s.rhs = copy_expr(mfa_at(l_ret->lst, i));
            
            vec_push_back(frm->G, &s);
        }

        if (is_var_e(L->rhs) || is_wild_e(L->rhs)) {//todo, remove is_wild_e
            //add sub on L for R's tail
            mf_array* l_R = R->rhs->e.a.data.vl.v.l.lst;
            substitution s2 = alloc_sub();
           *s2.lhs = copy_expr(L->lhs);
           *s2.rhs = make_list_e(false);
            //go backwards
            for (int i = mfa_card(l_R)-1; i >= start; i--) {
            //for (int i = start; i < mfa_card(l_R); i++) {
                add_to_list(&s2.rhs->e.a.data.vl.v.l, mfa_at(l_R, i));
            }
            vec_push_back(frm->G, &s2);
        }
    } else {
        //substitution* R = get_sub_frm_i(frm, call_sequ, vec_size(fc->params)-1);
        substitution s;
        s.lhs = malloc(sizeof(expr));
        s.rhs = malloc(sizeof(expr));
       *s.lhs = copy_expr(R->lhs);
       *s.rhs = copy_expr(L->rhs);
        //go backwards..
        for (int i = vec_size(fc->params)-3; i >= 0; i--) {//-3 = 1 ret var + 1 target list
            //note, params do not unify... fix latter
            substitution* param = get_sub_frm_i(frm, call_sequ, i);
            if (param == NULL) continue;
            outcome res = add_to_list(&s.rhs->e.a.data.vl.v.l, param->rhs);
            if (res == o_fail) return o_fail;
        }
        if (compare_atoms_e(s.lhs, s.rhs)) {
            free_expr(s.lhs);
            free_expr(s.rhs);
        } else {
            vec_push_back(frm->G, &s);
        }
    }
#ifdef UNIFY_DEBUG
    printf("DEBUG :: cons : after\n");
    dump_frame(frm);
#endif
    return o_pass;
}

//one of the returning bind variables is unneccesary, and is also the wrong call_sequ
//fix by rewriting and examining the single passed var, then adding sub to it on rhs
outcome plus_builtin(fcall* fc, frame* frm, int call_sequ) {
    if (vec_size(fc->params) != 3) {
        printf("Error. \"+/plus\" requires 3 operands.\n");
        return o_fail;
    }
    
    substitution* sX = get_sub_frm_i(frm, call_sequ, 0);
    substitution* sY = get_sub_frm_i(frm, call_sequ, 1);
    substitution* sR = get_sub_frm_i(frm, call_sequ, 2);
#ifdef UNIFY_DEBUG
    printf("DEBUG :: plus : plus called with:\n");
    if (sX != NULL) {
        dump_expr(*sX->lhs, false);printf(" = ");dump_expr(*sX->rhs, false);nl;
    }
    if (sY != NULL) {
        dump_expr(*sY->lhs, false);printf(" = ");dump_expr(*sY->rhs, false);nl;
    }
    if (sR != NULL) {
        dump_expr(*sR->lhs, false);printf(" = ");dump_expr(*sR->rhs, false);nl;
    }
#endif

    //pretty bad
    if (is_numeric_e(sX->rhs) && is_numeric_e(sY->rhs) && is_numeric_e(sR->rhs)) {
        //check
#ifdef UNIFY_DEBUG
                printf("DEBUG :: plus : checking:\n");
                printf("%i + %i == %i ; ans:%i\n",
                    sX->rhs->e.a.data.vl.v.i,
                    sY->rhs->e.a.data.vl.v.i,
                    sR->rhs->e.a.data.vl.v.i,
                    sX->rhs->e.a.data.vl.v.i + sY->rhs->e.a.data.vl.v.i
                );
#endif
        if (sX->rhs->e.a.data.vl.v.i + sY->rhs->e.a.data.vl.v.i == sR->rhs->e.a.data.vl.v.i) {
            return o_pass;
        }
#ifdef UNIFY_DEBUG
        printf("DEBUG :: plus : returned as a failure\n");
#endif
        return o_fail;

    } else if (is_numeric_e(sX->rhs) && is_numeric_e(sY->rhs) && is_var_e(sR->rhs)) {
        ///regular x + y = r
        //result var
        substitution s;
        s.lhs = malloc(sizeof(expr));
        s.rhs = malloc(sizeof(expr));
       *s.lhs = copy_expr(sR->rhs);
       *s.rhs = make_int_e(sX->rhs->e.a.data.vl.v.i + sY->rhs->e.a.data.vl.v.i);
        vec_push_back(frm->G, &s);

#ifdef UNIFY_DEBUG
        printf("DEBUG :: plus : successful and returning : %i eg:\n", s.rhs->e.a.data.vl.v.i);
        expr sub1 = {.type = e_equ, .e.e = s};
        dump_expr(sub1, true); nl;
#endif
        return o_pass;

    } else if (is_numeric_e(sX->rhs) && is_var_e(sY->rhs) && is_numeric_e(sR->rhs)) {
        ///subtraction y = r - x
        //result var
        substitution s;
        s.lhs = malloc(sizeof(expr));
        s.rhs = malloc(sizeof(expr));
       *s.lhs = copy_expr(sY->rhs);
       *s.rhs = make_int_e(sR->rhs->e.a.data.vl.v.i - sX->rhs->e.a.data.vl.v.i);
        vec_push_back(frm->G, &s);

#ifdef UNIFY_DEBUG
        printf("DEBUG :: plus : successful and returning : %i\n", s.rhs->e.a.data.vl.v.i);
        expr sub1 = {.type = e_equ, .e.e = s};
        dump_expr(sub1, true); nl;
#endif
        return o_pass;

    } else if (is_var_e(sX->rhs) && is_numeric_e(sY->rhs) && is_numeric_e(sR->rhs)) {
        ///subtraction x = r - y
        //result var 
        substitution s;
        s.lhs = malloc(sizeof(expr));
        s.rhs = malloc(sizeof(expr));
       *s.lhs = copy_expr(sX->rhs);
       *s.rhs = make_int_e(sR->rhs->e.a.data.vl.v.i - sY->rhs->e.a.data.vl.v.i);
        vec_push_back(frm->G, &s);

#ifdef UNIFY_DEBUG
        printf("DEBUG :: plus : successful and returning : %i\n", s.rhs->e.a.data.vl.v.i);
        expr sub1 = {.type = e_equ, .e.e = s};
        dump_expr(sub1, true); nl;
#endif
        return o_pass;

    } else {
#ifdef UNIFY_DEBUG
        printf("Info. Sorry that permutation of \"+\" operands has not yet been implemented.\n");
        printf("Namely:\n");
        dump_expr(*sX->lhs, true);printf(" = ");dump_expr(*sX->rhs, false);nl;
        dump_expr(*sY->lhs, true);printf(" = ");dump_expr(*sY->rhs, false);nl;
        dump_expr(*sR->lhs, true);printf(" = ");dump_expr(*sR->rhs, false);nl;
#endif
        //return o_undet;
        return o_fail;
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
    } else if (strcmp(fc->name, "cons") == 0) {
        return cons_builtin(fc, frm, call_sequ);
    } else if (strcmp(fc->name, "at_index") == 0) {
        return at_index_builtin(fc, frm, call_sequ);
    } else if (strcmp(fc->name, "at_set") == 0) {
        return at_set_builtin(fc, frm, call_sequ);
    } else if (strcmp(fc->name, "ref") == 0) {
        return ref_builtin(fc, frm, call_sequ);
    } else if (strcmp(fc->name, "card") == 0) {
        return card_builtin(fc, frm, call_sequ);
    } else {
        printf("Info. Sorry, builtin function \"%s\" is not yet implemented.\n", fc->name);
        return o_fail;
    }
}

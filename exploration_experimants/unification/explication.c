
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "explication.h"
#include "utils.h"
#include "mlog_array.h"

#include "debug_stuff.h"

void explic_func(function* f, vector* func_defs) {
    if (f->fully_defined) return;
    decompose_equs(f);//doing decomp equs first, not sure what the implications are yet.
    explic_func_expr(f, func_defs);
    explic_func_head(f);
    f->fully_defined = true;
    //add to choice points
    choice_point* cp = get_cpoint(func_defs_cp, f);
    if (cp == NULL) {
        cp = malloc(sizeof(choice_point));
        cp->functions = new_vector(1, sizeof(function));
        vec_push_back(cp->functions, f);
        vec_push_back(func_defs_cp, cp);
        free(cp);
    } else {
        vec_push_back(cp->functions, f);
    }
}

void explic_func_expr(function* f, vector* func_defs) {
    int unique = 0;
    vector* moved_fcalls = new_vector(10, sizeof(expr));
    vector* singletons = get_var_singles(f, true);
    rec_explic_func_expr(&f->e, &unique, moved_fcalls, singletons, func_defs, f);
    //and append moved fcalls
    for (int i = 0; i < vec_size(moved_fcalls); i++) {
        expr* fc = vec_at(moved_fcalls, i);
        append_expr(&f->e, fc);
    }
}

void rec_explic_func_expr(expr* e, int* unique, vector* fc_to_move, vector* singletons, vector* func_defs, function* root) {
    if (is_val_e(e)) {
        //replace with equality and unique var
        expr new_left  = make_var_e(unique_name_incr(unique));
        expr new_right = *e;
        e->type = e_equ;
        e->e.e.lhs = malloc(sizeof(expr));
        e->e.e.rhs = malloc(sizeof(expr));
       *e->e.e.lhs = new_left;
       *e->e.e.rhs = new_right;
    } else if (is_and_e(e)) {
        vector* ands = e->e.n.ands;
        for (int i = 0; i < vec_size(ands); i++) {
            rec_explic_func_expr(vec_at(ands, i), unique, fc_to_move, singletons, func_defs, root);
        }
    } else if (is_fcall_e(e)) {
        //function* def = get_fdef_arity(func_defs, e->e.f.name, size(e->e.f.params));
        //first attempt to fine a fully defined version
        function* def = get_fdef_defined(func_defs, e->e.f.name);
        if (def == NULL) {//then resort to non-fully defined
            def = get_fdef(func_defs, e->e.f.name);
        }
        if (def == NULL) {
            printf("Error, unknown function %s with arity %i.\n", e->e.f.name, vec_size(e->e.f.params));
            return;
        }
        explic_func(def, func_defs);//DANGER, if no base case is defined, then infinit loop
        explic_func_params(&e->e.f, unique, fc_to_move, singletons, func_defs, root);
        explic_callsite(&e->e.f, unique, NULL, func_defs);
    } else if (is_equ_e(e)) {
        rec_explic_func_equ(e, unique, fc_to_move, singletons, func_defs, root);
    } else if (is_equ_chain_e(e)) {
        vector* equs = e->e.ec.equs;
        for (int i = 0; i < vec_size(equs); i++) {
            rec_explic_func_expr(vec_at(equs, i), unique, fc_to_move, singletons, func_defs, root);
        }
    }
}

//this is truly horrific and needs to be redone, badly..
void rec_explic_func_equ(expr* e, int* unique, vector* fc_to_move, vector* singles, vector* func_defs, function* root) {
    if (is_equ_e(e)) {
        rec_explic_func_equ(e->e.e.lhs, unique, fc_to_move, singles, func_defs, root);//note, this was e->e.n.lhs
        rec_explic_func_equ(e->e.e.rhs, unique, fc_to_move, singles, func_defs, root);
    } else if (is_equ_chain_e(e)) {
        vector* equs = e->e.ec.equs;
        for (int i = 0; i < vec_size(equs); i++) {
            rec_explic_func_equ(vec_at(equs, i), unique, fc_to_move, singles, func_defs, root);
        }
    } else if (is_fcall_e(e)) {
        rec_explic_func_expr(e, unique, fc_to_move, singles, func_defs, root);
        //ToDo, if all params are already bound, then bind the outcome of func
        //to lhs instead, ie bot/top

        //move fcall out
        vec_push_back(fc_to_move, e);
        //get new singles from root def
        vector* new_singles = get_var_singles(root, true);

        expr unbounds = tuplise_params(e->e.f.params, new_singles);
       *e = copy_expr(&unbounds);
        free_vector(new_singles);
    }
}

//old
void dump_vector_str(vector* vec) {
    for (int i = vec_size(vec)-1; i >= 0 ; i--) {
        char** el = vec_at(vec, i);
        printf("vec at :%i , :%s\n", i, *el);
    }
}

expr tuplise_params(vector* params, vector* singles) {
    expr ex;
    ex.type = e_builtin;//not the best
    for (int i = 0; i < vec_size(params); i++) {
        expr* pr = vec_at(params, i);
        if (is_var_e(pr) && vec_contains_string(singles, pr->e.a.data.vr.symbol)) {
            if (ex.type == e_builtin) {
                ex = copy_expr(pr);
            } else {
                expr new_ex = copy_expr(pr);
                append_expr(&ex, &new_ex);//check
            }
        }
    }
    if (is_and_e(&ex)) {
        return wrap_and_t(ex.e.n);
    } else {
        return ex;
    }
}

void explic_func_params(fcall* fc, int* unique, vector* fc_to_move, vector* singletons, vector* func_defs, function* root) {
    for (int i = 0; i < vec_size(fc->params); i++) {
        expr* e = vec_at(fc->params, i);
        expr param = *e;
        if (is_fcall_e(&param)) {
            //WARN, this will just return the first found match reguardless of arity
            //maybe get the fdef with the smallest matching arity?
            function* def = get_fdef(func_defs, param.e.f.name);
            if (def == NULL) {
                printf("Error, unknown function %s with arity %i.\n", e->e.f.name, vec_size(e->e.f.params));
                return;
            }
            explic_func(def, func_defs);
            explic_func_params(&param.e.f, unique, fc_to_move, singletons, func_defs, root);
            vector* newly_generated = new_vector(0, sizeof(expr));
            explic_callsite(&param.e.f, unique, newly_generated, func_defs);
            ////decompose func
            //push_back newly generated
            for (int j = 0; j < vec_size(newly_generated); j++) {
                vec_insert_at(fc->params, i, vec_at(newly_generated, j)); i++;
            }
            free_vector(newly_generated);
            //push_back singletons
            for (int j = 0; j < vec_size(param.e.f.params); j++) {/////////NOTE. I think this may be dead code...
                expr* pparam = vec_at(param.e.f.params, j);
                for (int k = 0; k < vec_size(singletons); k++) {
                    char** sing = vec_at(singletons, k);
                    if (is_var_e(pparam)) {
                        if (strcmp(*sing, pparam->e.a.data.vr.symbol) == 0) {
                            //is a singleton
                            char* temp = malloc(sizeof(char)*(strlen(*sing)+1));
                            sprintf(temp, "%s", *sing);////added this..
                            printf("###about to add : %s\n", *sing);
                            expr new_var = make_var_e(temp);
                            vec_insert_at(fc->params, i, &new_var); i++;
                        }
                    }   
                }
            }
            vec_push_back(fc_to_move, &param);
            vec_remove_at(fc->params, i); i--;
        }
    }
}

//newly_generated is optional, if not null, it will contain the newly
//generated variables
void explic_callsite(fcall* fc, int* unique, vector* newly_generated, vector* func_defs) {
    //WARN, this will just return the first found match reguardless of arity
    //maybe get the fdef with the smallest matching arity?
    function* def = get_fdef(func_defs, fc->name);
    if (def == NULL) {
        printf("Error, unknown function %s with arity %i.\n", fc->name, vec_size(fc->params));
        return;
    }
    while (vec_size(fc->params) < vec_size(def->params)) {
        expr new_name = make_var_e(unique_name_incr(unique));
        vec_push_back(fc->params, &new_name);
        if (newly_generated != NULL) {
            vec_push_back(newly_generated, &new_name);
        }
    }
}

void explic_func_head(function* f) {
    vector* f_singles = get_var_singles(f, false);
    for (int i = 0; i < vec_size(f_singles); i++) {
        char** s = vec_at(f_singles, i);
        //*s = regular char*
        char*  s_new_param = malloc(sizeof(char)*strlen(*s)+1);
        strcpy(s_new_param, *s);
        atom a_new_param = {a_var, .data.vr.symbol = s_new_param};
        vec_push_back(f->params, &a_new_param);
    }

    free_vector(f_singles);
}

void decompose_equs(function* f) {
    vector* exs = f->e.e.n.ands;
    for (int i = 0; i < vec_size(exs); i++) {
        expr* ei = vec_at(exs, i);
        if (is_equ_chain_e(ei)) {
            vector* ech = ei->e.ec.equs;
            expr* first = vec_at(ech, 0);
            for (int j = 1; j < vec_size(ech); j++) {
                expr* next = vec_at(ech, j);
                equality eq;
                eq.lhs = malloc(sizeof(expr));
               *eq.lhs = copy_expr(first);
                eq.rhs = malloc(sizeof(expr));
               *eq.rhs = copy_expr(next);
                expr eq_e = {.type = e_equ, .e.e = eq};
                vec_push_back(exs, &eq_e);
            }
            vec_remove_at(exs, i); i--;
        }
    }
}

/*
void rec_decompose_equs_chain(expr* ex, expr equ_root, expr* func_root) {
    if (is_equ_e(ex)) {
        //lhs
        expr new_equ;
        new_equ.type = e_equ;
        expr new_lhs = copy_expr(&equ_root);
        new_equ.e.e.lhs = malloc(sizeof(expr));
       *new_equ.e.e.lhs = new_lhs;
        expr new_rhs = copy_expr(ex->e.e.lhs);
        new_equ.e.e.rhs = malloc(sizeof(expr));
       *new_equ.e.e.rhs = new_rhs;
        append_expr(func_root, &new_equ);
        //rhs
        rec_decompose_equs_chain(ex->e.e.rhs, equ_root, func_root);
    } else {
        expr new_eq;
        new_eq.type = e_equ;
        expr new_lhs = copy_expr(&equ_root);
        new_eq.e.e.lhs = malloc(sizeof(expr));
       *new_eq.e.e.lhs = new_lhs;
        expr new_rhs = copy_expr(ex);
        new_eq.e.e.rhs = malloc(sizeof(expr));
       *new_eq.e.e.rhs = new_rhs;
        append_expr(func_root, &new_eq);
    }
}

//horrific, please make better
void rec_decompose_equs(expr* ex, expr* func_root) {
    if (is_and_e(ex)) {
        printf("current expression is an and\n");
        rec_decompose_equs(ex->e.n.lhs, func_root);
        //replace with dummy
        //if (is_equ_e(ex->e.n.lhs) && is_equ_e(ex->e.n.lhs->e.e.rhs)) {
        //    *ex->e.n.lhs = make_int_e(1);
        //}
        rec_decompose_equs(ex->e.n.rhs, func_root);
        
    } else if (is_equ_e(ex)) {//root
        printf("found a root equ\n");
        expr orig_lhs = copy_expr(ex->e.e.lhs);
        if (is_equ_e(ex->e.e.rhs)) {//rhs
            printf("processing tail\n");
            rec_decompose_equs_chain(ex->e.e.rhs, orig_lhs, func_root);
           *ex->e.e.lhs->e.e.rhs = orig_lhs;
        }
    }
}
void decompose_equs(function* f) {
    rec_decompose_equs(&f->e, &f->e);
}*/

//probably dont need
void free_var_singles(vector* vec) {
    if (vec == NULL) return;
    for (int i = 0; i < vec_size(vec); i++) {
        char** sing = vec_at(vec, i);
        free(*sing);
    }
    free_vector(vec);
}

vector* get_var_singles(function* f, bool globals) {
    //varname, count
    map* mp = new_map(sizeof(char*), sizeof(int));
    mp->el1_comparator = string_compare;
    rec_get_var_singles_e(&f->e, mp);
    ////cheating by using global externs...
    if (globals) for (int i = 0; i < vec_size(global_defs); i++) {
        rec_get_var_singles_e(vec_at(global_defs, i), mp);
    }
    ////end cheat

    check_var_singles_params(f->params, mp);
    vector* singles = extract_singles_map(mp);
    
    //add the intersect of the globals, as the expr may refer to them,
    //nolonger making them single
    expr g_temp = {.type = e_and, .e.n.ands = global_defs};
    vector* global_sings = get_var_singles_e(&g_temp);
    for (int i = 0; i < vec_size(singles); i++) {
        char** sing = vec_at(singles, i);
        if (vec_contains_string(global_sings, *sing)) {
            //remove
            vec_remove_at(singles, i); i--;
        }
    }
    
    return singles;
}

//root expr, can be and of exprs
//eg globals/f.expr
vector* get_var_singles_e(expr* ex) {
    //varname, count
    map* mp = new_map(sizeof(char*), sizeof(int));
    mp->el1_comparator = string_compare;
    rec_get_var_singles_e(ex, mp);
    return extract_singles_map(mp);
}

vector* extract_singles_map(map* mp) {
    vector* singletons = new_vector(0, sizeof(char*));
    for (int i = 0; i < map_size(mp); i++) {
        pair* kv = map_at_index(mp, i);
        if (*(int*)kv->snd == 1) {
            //push_back(singletons, *(char**)kv->fst);//check this.
            vec_push_back(singletons, (char**)kv->fst);//check this.
        }
    }
    free_map(mp);
    return singletons;
}

void rec_get_var_singles_e(expr* e, map* mp) {
    if (is_var_e(e)) {
        char* v_name = e->e.a.data.vr.symbol;
        if (map_contains_key(mp, &v_name)) {
            (*(int*)snd(mp, &v_name))++;
        } else {
            int one = 1;
            map_add(mp, &v_name, &one);
        }
    } else if (is_and_e(e)) {
        vector* ands = e->e.n.ands;
        for (int i = 0; i < vec_size(ands); i++) {
            rec_get_var_singles_e(vec_at(ands, i), mp);
        }
    } else if (is_equ_e(e)) {
        rec_get_var_singles_e(e->e.e.lhs, mp);
        rec_get_var_singles_e(e->e.e.rhs, mp);
    } else if (is_equ_chain_e(e)) {
        vector* equs = e->e.ec.equs;
        for (int i = 0; i < vec_size(equs); i++) {
            rec_get_var_singles_e(vec_at(equs, i), mp);
        }
    } else if (is_fcall_e(e)) {
        vector* ps = e->e.f.params;
        for (int i = 0; i < vec_size(ps); i++) {
            rec_get_var_singles_e(vec_at(ps, i), mp);
        }
    } else if (is_tuple_e(e)) {
        vector* tps = e->e.t.n.ands;
        for (int i = 0; i < vec_size(tps); i++) {
            rec_get_var_singles_e(vec_at(tps, i), mp);
        }
    } else if (is_list_e(e)) {
        mf_array* lst = e->e.a.data.vl.v.l.lst;
        if (lst == NULL) {
            printf("An empty list contains no vars...\n");
            return;
        }
        for (int i = 0; i < mfa_card(lst); i++) {
            rec_get_var_singles_e(mfa_at(lst, i), mp);
        }
        //printf("Info. Checking for variable singles in lists is not yet implemented.\n");
    }
}

void check_var_singles_params(vector* params, map* mp) {
    for (int i = 0; i < vec_size(params); i++) {
        atom* pr = vec_at(params, i);
        if (is_var_a(pr)) {
            char* v_name = pr->data.vr.symbol;
            if (map_contains_key(mp, &v_name)) {
                (*(int*)snd(mp, &v_name))++;
            }
        }//doesnt add new singles, only increments existing ones
    }
}

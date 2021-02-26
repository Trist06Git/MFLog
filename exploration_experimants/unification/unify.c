
#include <stdio.h>
#include <stdlib.h>

#include "unify.h"
#include "utils.h"
#include "debug_stuff.h"

extern fcall fc_main;

#define UNIFY_DEBUG

void entry(vector* func_defs_cp, vector* globals) {
    choice_point* main_cp = get_cpoint_na(func_defs_cp, "main", 1);
    if (main_cp == NULL) {
        printf("Error. Could not find a main with arity 1.\n  >1 arity not yet supported.\n");
        return;
    }
    int call_sequ = 0;
    function* main_f = vec_at(main_cp->functions, 0);
    
    frame* first_frame = init_frame(main_f, &fc_main, globals, &call_sequ);

    outcome entry_res = unify(first_frame, func_defs_cp, globals, &call_sequ);
    //only unifying, not binding return vals

    if (entry_res == o_pass) {
        printf("\"main\" returned a pass, with answers:\n");
        vector* res = head_results(first_frame, main_f);
        for (int i = 0; i < vec_size(res); i++) {
            expr* r = vec_at(res, i);
            dump_expr(*r, true);
            printf("\n");
        }
    } else if (entry_res == o_answers) {
        printf("\"main\" returned a pass, with no answers.\n");
    } else {
        printf("\"main\" returned with a failure.\n");
    }

    free_frame(first_frame);
}

outcome call(frame_call* fr_c, frame* prev_frm, vector* func_defs_cp, vector* globals, int* call_sequ) {
#ifdef UNIFY_DEBUG
    printf("DEBUG :: Calling frame for \"%s\"\n", fr_c->fc.name);
#endif
    fcall fc = fr_c->fc;
    choice_point* f_cp = get_cpoint_na(func_defs_cp, fc.name, vec_size(fc.params));
    if (f_cp == NULL) {
        printf("Error. Could not find function \"%s\" with arity %i\n", fc.name, vec_size(fc.params));
        return o_fail;
    }

    //only get the first answer
    if (fc.res_set == rs_first) {
        function* first_cp = vec_at(f_cp->functions, 0);
        if (first_cp->e.type == e_builtin) {
            printf("Info. Builtin functions are not yet implemented\n");
            return o_pass;
        }
        int fs_sequ = fr_c->call_sequence;
        frame* next_frm = init_frame(first_cp, &fc, globals, &fs_sequ);
        outcome res = unify(next_frm, func_defs_cp, globals, call_sequ);
        if (res == o_fail) return o_fail;
        //consolidate previous frame with next frame
        //G1 = G1 + G1 N G2, where N = intersect, G1 = prev_frm, G2 = next_frm
        for (int g2 = 0; g2 < vec_size(next_frm->G); g2++) {
            substitution* s = vec_at(next_frm->G, g2);
            if ((is_var_e(s->lhs) && compare_decomp_sequ(s->lhs, fr_c->call_sequence))
                ||
                (is_var_e(s->rhs) && compare_decomp_sequ(s->rhs, fr_c->call_sequence)))
            {
                substitution ret_s = copy_equ(s);
                vec_push_back(prev_frm->G, &ret_s);
            }
        }
        free_frame(next_frm);
    } else {
        printf("Info. Only the first choice point answer is currently implemented\n");
        printf("for function: %s/%i\n", fc.name, vec_size(fc.params));
        return o_pass;
    }

    return o_pass;//need a way of returning o_answers..
}

outcome unify(frame* frm, vector* func_defs_cp, vector* globals, int* call_sequ) {
#ifdef UNIFY_DEBUG
    printf("DEBUG :: Before unify:\n");
    dump_frame(frm);
#endif
    //first call funcs. g, f, etc
    for (int i = 0; i < vec_size(frm->next_calls); i++) {//change this to remove at back. latter
        frame_call* frc = vec_at(frm->next_calls, i);
        if (call(frc, frm, func_defs_cp, globals, call_sequ) == o_fail) {
            return o_fail;
        }
    }
    //Note, swap has been moved into eliminate
    //done before each source substitution
    //then eliminate
    eliminate(frm);
    //decompose tuples
    decompose(frm);
    //then delete 
    delete_g(frm);
    //then conflict
    outcome res = conflict(frm);
#ifdef UNIFY_DEBUG
    printf("DEBUG :: After unify:\n");
    dump_frame(frm);
#endif
    return res;
}

frame* init_frame(function* f, fcall* fc, vector* globals, int* call_sequ) {
    frame* frm = malloc(sizeof(frame));
    frm->G = new_vector(0, sizeof(substitution));
    frm->last_result = o_pass;
    frm->next_calls = new_vector(0, sizeof(frame_call));

    //setup uniquely named parameter variables, so as not to
    //clash with the caller
    for (int p = 0; p < vec_size(f->params); p++) {
        atom param = copy_atom(vec_at(f->params, p));
        substitution call_binds;
        call_binds.lhs = malloc(sizeof(expr));
        call_binds.rhs = malloc(sizeof(expr));
       *call_binds.lhs = make_var_e(decomp_name(call_sequ, &p));
       *call_binds.rhs = wrap_atom(param);
        vec_push_back(frm->G, &call_binds);
    }
    //add passed params
    for (int p = 0; p < vec_size(fc->params); p++) {
        expr* p_ex = vec_at(fc->params, p);
        if (is_val_e(p_ex)) {
            expr passed = copy_expr(p_ex);
            substitution call_bind;
            call_bind.lhs = malloc(sizeof(expr));
            call_bind.rhs = malloc(sizeof(expr));
           *call_bind.lhs = make_var_e(decomp_name(call_sequ, &p));
           *call_bind.rhs = passed;
            vec_push_back(frm->G, &call_bind);
        }
    }
    (*call_sequ)++;
    //when calling sub functions, they must have different vars
    //from this functions return vars

    //add all of f's equations
    //rec_add_expr(frm, &f->e, call_sequ);
    add_frame_exprs(frm, &f->e, call_sequ);
    //add global scope equations
    for (int i = 0; i < vec_size(globals); i++) {
        //rec_add_expr(frm, vec_at(globals, i), call_sequ);
        expr gls;
        gls.type = e_and;
        gls.e.n.ands = globals;
        add_frame_exprs(frm, &gls, call_sequ);
    }

    return frm;
}

void free_frame(frame* frm) {
    for (int g = 0; g < vec_size(frm->G); g++) {
        free_equ(vec_at(frm->G, g));
    }
    free_vector(frm->G);
    for (int i = 0; i < vec_size(frm->next_calls); i++) {
        frame_call* fc = vec_at(frm->next_calls, i);
        free_fcall(&fc->fc);
    }//a frame may have failed early, so all fcall may not have been removed
    free_vector(frm->next_calls);
    free(frm);
}

void add_frame_exprs(frame* frm, expr* e, int* call_sequ) {
    if (is_and_e(e)) {//may conflict latter when adding real ands
        vector* ands = e->e.n.ands;
        for (int i = 0; i < vec_size(ands); i++) {
            add_frame_exprs(frm, vec_at(ands, i), call_sequ);
        }
    } else if (is_equ_e(e)) {//by now all equ chains should have decomposed
        substitution s = copy_equ(&e->e.e);
        vec_push_back(frm->G, &s);

    } else if (is_atom_e(e)) {//dead code
        substitution s;
        s.lhs = malloc(sizeof(expr));
        s.rhs = malloc(sizeof(expr));
       *s.lhs = make_query(&e->e.a);
       *s.rhs = copy_expr(&debug_dummy_var);
        //dead code
    } else if (is_fcall_e(e)) {
        vector* params = e->e.f.params;
        for (int i = 0; i < vec_size(params); i++) {
            substitution s;
            s.lhs = malloc(sizeof(expr));
            s.rhs = malloc(sizeof(expr));
           *s.lhs = make_var_e(decomp_name(call_sequ, &i));
           *s.rhs = copy_expr(vec_at(params, i));
            vec_push_back(frm->G, &s);
        }
        frame_call f;
        f.call_sequence = *call_sequ;
        f.fc = e->e.f;
        vec_push_back(frm->next_calls, &f);
        (*call_sequ)++;
    }
}

/*
//does copying... because we need to modify the substitutions.
void add_frame_exprs(frame* frm, expr* e, int* call_sequ) {
    vector* exs = e->e.n.ands;
    for (int i = 0; i < vec_size(exs); i++) {
        expr* ei = vec_at(exs, i);
        
        if (is_equ_e(ei)) {//by now all equ chains should have decomposed
            substitution s = copy_equ(&ei->e.e);
            printf("about to add to frame:\n");
            expr temp = {.type = e_equ, .e.e = s};
            dump_expr(temp, true); nl;
            vec_push_back(frm->G, &s);

        } else if (is_atom_e(ei)) {//dead code
            substitution s;
            s.lhs = malloc(sizeof(expr));
            s.rhs = malloc(sizeof(expr));
           *s.lhs = make_query(&ei->e.a);
           *s.rhs = copy_expr(&debug_dummy_var);
            //dead code
        } else if (is_fcall_e(ei)) {
            vector* params = ei->e.f.params;
            for (int i = 0; i < vec_size(params); i++) {
                substitution s;
                s.lhs = malloc(sizeof(expr));
                s.rhs = malloc(sizeof(expr));
               *s.lhs = make_var_e(decomp_name(call_sequ, &i));
               *s.rhs = copy_expr(vec_at(params, i));
                vec_push_back(frm->G, &s);
            }
            frame_call f;
            f.call_sequence = *call_sequ;
            f.fc = ei->e.f;
            vec_push_back(frm->next_calls, &f);
           *call_sequ++;

        } else if (is_and_e(ei)) {
            //rec_add_expr(frm, ei->e.n.lhs, call_sequ);
            //rec_add_expr(frm, ei->e.n.rhs, call_sequ);
            //not done yet
        }
    }
}*/

/*
//does copying... because we need to modify the substitutions.
void rec_add_expr(frame* frm, expr* ex, int* call_sequ) {
    if (is_equ_e(ex)) {//only supports x = y for now, no x = z = y...
        substitution s = copy_equ(&ex->e.e);
        vec_push_back(frm->G, &s);
    } else if (is_atom_e(ex)) {
        substitution s;
        s.lhs = malloc(sizeof(expr));
        s.rhs = malloc(sizeof(expr));
       *s.lhs = make_query(&ex->e.a);
       *s.rhs = copy_expr(&debug_dummy_var);
    } else if (is_fcall_e(ex)) {
        vector* params = ex->e.f.params;
        for (int i = 0; i < vec_size(params); i++) {
            substitution s;
            s.lhs = malloc(sizeof(expr));
            s.rhs = malloc(sizeof(expr));
           *s.lhs = make_var_e(decomp_name(call_sequ, &i));
           *s.rhs = copy_expr(vec_at(params, i));
            vec_push_back(frm->G, &s);
        }
        frame_call f;
        f.call_sequence = *call_sequ;
        f.fc = ex->e.f;
        vec_push_back(frm->next_calls, &f);
       *call_sequ++;
    } else if (is_and_e(ex)) {
        rec_add_expr(frm, ex->e.n.lhs, call_sequ);
        rec_add_expr(frm, ex->e.n.rhs, call_sequ);
    }
}*/

//decompose tuples
void decompose(frame* frm) {
    for (int g = 0; g < vec_size(frm->G); g++) {
        substitution* sub = vec_at(frm->G, g);
        if (is_tuple_e(sub->lhs) &&
            is_tuple_e(sub->rhs) &&
            tuple_size_e(sub->lhs) == tuple_size_e(sub->rhs)
           ) {
               vector* tupe_lhs = sub->lhs->e.t.n.ands;
               vector* tupe_rhs = sub->rhs->e.t.n.ands;
               for (int i = 0; vec_size(tupe_lhs); i++) {
                   expr* el_lhs = vec_at(tupe_lhs, i);
                   expr* el_rhs = vec_at(tupe_rhs, i);
                   substitution new_sub;
                  *new_sub.lhs = copy_expr(el_lhs);
                  *new_sub.rhs = copy_expr(el_rhs);
                   swap_substitution(&new_sub);
                   vec_push_back(frm->G, &new_sub);
               }
               //keeping the old tuple, because the structure
               //is semantically important
           }
    }
}

//only swap if var is on rhs, skip otherwise
void swap_substitution(substitution* sub) {
    if (!is_var_e(sub->lhs) && is_var_e(sub->rhs)) {
        expr* temp = sub->lhs;
        sub->lhs = sub->rhs;
        sub->rhs = temp;
    }/* else if (!is_var_e(sub->lhs) && is_equ_e(sub->rhs)) {
        swap_equs(&sub->rhs->e.e, sub->lhs);
    }*///see above func
}

//this would be cheaper if done from end to start with remove_back,
//avoiding middle remove_at
void delete_g(frame* frm) {
    for (int g = 0; g < vec_size(frm->G); g++) {
        substitution* sub = vec_at(frm->G, g);
        if (compare_atoms_e(sub->lhs, sub->rhs)) {
            free_equ(sub);
            vec_remove_at(frm->G, g); g--;
        }
    }
}

outcome conflict(frame* frm) {
    for (int g = 0; g < vec_size(frm->G); g++) {
        substitution* sub = vec_at(frm->G, g);
        if (is_val_e(sub->lhs) && is_val_e(sub->rhs)) {
            if (!compare_atoms_e(sub->lhs, sub->rhs)) {
                frm->last_result = o_fail;
                return o_fail;
            }
        } else if (is_tuple_e(sub->lhs) &&
                   is_tuple_e(sub->rhs) &&
                   tuple_size_e(sub->lhs) != tuple_size_e(sub->rhs)
                  ) {
                      return o_fail;
        }
        //also check the contents of the tuples...
    }
    return o_pass;
}

//old
void equ_eliminate(expr* equ_ch, const expr* var, const expr* val) {
    vector* equs = equ_ch->e.ec.equs;
    for (int i = 0; i < vec_size(equs); i++) {
        expr* ei = vec_at(equs, i);
        if (is_var_e(ei) && compare_atoms_e(ei, var)) {
            free_expr(ei);
           *ei = copy_expr(val);
        }
    }
}

//maybe add an early conflict check for a small speed boost??
void eliminate(frame* frm) {
    for (int g = 0; g < vec_size(frm->G); g++) {
        substitution* sub = vec_at(frm->G, g);
        swap_substitution(sub);
        expr* sub_var = sub->lhs;
        expr* sub_val = sub->rhs;
        if (is_var_e(sub_var)) {
            //eliminate in G
            for (int g2 = 0; g2 < vec_size(frm->G); g2++) {
                if (g2 == g) continue;//skip self
                substitution* g2_sub = vec_at(frm->G, g2);
                if (is_var_e(g2_sub->lhs) && compare_atoms_e(g2_sub->lhs, sub_var)) {
                    //they are the same, so substitute
                    free_expr(g2_sub->lhs);
                   *g2_sub->lhs = copy_expr(sub_val);
                } else if (is_tuple_e(g2_sub->lhs)) {
                    tuple_eliminate(&g2_sub->lhs->e.t, sub_var, sub_val);
                }
                if (is_var_e(g2_sub->rhs) && compare_atoms_e(g2_sub->rhs, sub_var)) {
                    //same, so sub
                    free_expr(g2_sub->rhs);
                   *g2_sub->rhs = copy_expr(sub_val);
                } else if (is_tuple_e(g2_sub->rhs)) {
                    tuple_eliminate(&g2_sub->rhs->e.t, sub_var, sub_val);
                } else if (is_equ_e(g2_sub->rhs)) {
                    //go down list in g2->rhs and elinate using sub_var, sub_val
                    //equ_eliminate(g2_sub->rhs, sub_var, sub_val);
                    //NOTE. this should now be dead code, as nested equs are
                    //treated as equ_chains
                    printf("BAD in eliminate() .. is_equ_e()");
                } else if (is_equ_chain_e(g2_sub->rhs)) {
                    //equ_eliminate(g2_sub->rhs, sub_var, sub_val);
                    //NOTE. this is also dead code, as equ chains are decomposed
                    //at explicate phase
                    printf("BAD in eliminate() .. is_equ_chain_e()");
                }
            }
        }
    }
}

void tuple_eliminate(tuple* tu, const expr* var, const expr* val) {
    for (int i = 0; i < vec_size(tu->n.ands); i++) {
        expr* ti = vec_at(tu->n.ands, i);

        if (is_var_e(ti) && compare_atoms_e(ti, var)) {
            //free_expr(ti);//this is bad, as the expr lives in the vector and cant be freed, but what about its sub elements?
           *ti = copy_expr(val);
        }
    }
}

outcome query(expr* ex) {
    return o_pass;//not done yet
}

vector* head_results(frame* frm, function* f) {
    vector* res = new_vector(1, sizeof(expr));
    for (int i = 0; i < vec_size(f->params); i++) {
        atom* atm = vec_at(f->params, i);
        expr ex = {.type = e_atom, .e.a = *atm};
        for (int g = 0; g < vec_size(frm->G); g++) {
            equality* eq = vec_at(frm->G, g);
            //at this point, only vars should be on the lhs
            if (compare_atoms_e(&ex, eq->lhs)) {
                expr eq_e = {.type = e_equ, .e.e = *eq};
                expr r = copy_expr(&eq_e);
                vec_push_back(res, &r);
            }
        }
    }
    return res;
}

char* outcome_to_string(const outcome* o) {
    switch (*o) {
        case o_answers : return answ_string  ;
        case o_pass    : return pass_string  ;
        case o_fail    : return fail_string  ;
        default        : return random_error ;
    }
}

void dump_frame(frame* frm) {
    printf("Frame has %i substitutions:\n", vec_size(frm->G));
    for (int g = 0; g < vec_size(frm->G); g++) {
        substitution* s = vec_at(frm->G, g);
        dump_expr(*s->lhs, true); printf(" = "); dump_expr(*s->rhs, false); nl;
    }
    printf("Sub calls:\n");
    for (int i = 0; i < vec_size(frm->next_calls); i++) {
        fcall* f = vec_at(frm->next_calls, i);
        printf("  "); dump_func_call(*f, true); nl;
    }
    printf("Result thus far: %s\n", outcome_to_string(&frm->last_result));
}

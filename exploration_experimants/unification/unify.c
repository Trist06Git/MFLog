
#include <stdio.h>
#include <stdlib.h>

#include "unify.h"
#include "utils.h"
#include "debug_stuff.h"

#define UNIFY_DEBUG

void entry(vector* func_defs_cp, vector* globals) {
    choice_point* main_cp = get_cpoint_na(func_defs_cp, "main", 1);
    if (main_cp == NULL) {
        printf("Error. Could not find a main with arity 1.\n  >1 arity not yet supported.\n");
        return;
    }
    int call_sequ = 0;
    function* main_f = vec_at(main_cp->functions, 0);
    frame* first_frame = init_frame(main_f, globals, &call_sequ);

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
        frame* next_frm = init_frame(first_cp, globals, call_sequ);
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
    //then swap
    for (int g = 0; g < vec_size(frm->G); g++) {
        swap_substitution(vec_at(frm->G, g));
    }
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

frame* init_frame(function* f, vector* globals, int* call_sequ) {
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
    (*call_sequ)++;
    //when calling sub functions, they must have different vars
    //from this functions return vars

    //add all of f's equations
    rec_add_expr(frm, &f->e, call_sequ);
    //add global scope equations
    for (int i = 0; i < vec_size(globals); i++) {
        rec_add_expr(frm, vec_at(globals, i), call_sequ);
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
}

//decompose tuples
void decompose(frame* frm) {
    for (int g = 0; g < vec_size(frm->G); g++) {
        substitution* sub = vec_at(frm->G, g);
        if (is_tuple_e(sub->lhs) &&
            is_tuple_e(sub->rhs) &&
            tuple_size_e(sub->lhs) == tuple_size_e(sub->rhs)
           ) {
                //first leaf
                substitution first;
                first.lhs = sub->lhs->e.t.n.lhs;
                first.rhs = sub->rhs->e.t.n.rhs;
                vec_push_back(frm->G, &first);
                //go down the list and pair the nodes up
                expr* node_a = sub->lhs->e.t.n.rhs;
                expr* node_b = sub->rhs->e.t.n.rhs;
                while (is_and_e(node_a) && is_and_e(node_b)) {
                   substitution pair;
                   pair.lhs = node_a->e.n.lhs;
                   pair.rhs = node_b->e.n.rhs;
                   vec_push_back(frm->G, &pair);
                   expr* temp_a = node_a; node_a = node_a->e.n.rhs; free(temp_a);
                   expr* temp_b = node_b; node_b = node_b->e.n.rhs; free(temp_b);
                }
                //last leaf
                substitution last;
                last.lhs = node_a;
                last.rhs = node_b;
                swap_substitution(&last);//Carefull...
                vec_push_back(frm->G, &last);
                //remove the tuple
                vec_remove_at(frm->G, g); g--;
        }
    }
}

//only swap if var is on rhs, skip otherwise
void swap_substitution(substitution* sub) {
    if (!is_var_e(sub->lhs) && is_var_e(sub->rhs)) {
        expr* temp = sub->lhs;
        sub->lhs = sub->rhs;
        sub->rhs = temp;
    }
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

void old_decompose(frame* frm) {
    for (int g = 0; g < vec_size(frm->G); g++) {
        substitution* sub = vec_at(frm->G, g);
        if (is_fcall_e(sub->lhs) && is_fcall_e(sub->rhs)) {
            fcall* lhs_f = &sub->lhs->e.f;
            fcall* rhs_f = &sub->rhs->e.f;
            for (int i = 0; i < vec_size(lhs_f->params); i++) {
                substitution new_sub;
                *new_sub.lhs = copy_expr(vec_at(lhs_f->params, i));
                *new_sub.rhs = copy_expr(vec_at(rhs_f->params, i));
                vec_push_back(frm->G, &new_sub);
            }
            vec_push_back(frm->next_calls, rhs_f);
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
    }
    return o_pass;
}

//maybe add an early conflict check for a small speed boost??
void eliminate(frame* frm) {
    for (int g = 0; g < vec_size(frm->G); g++) {
        substitution* sub = vec_at(frm->G, g);
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
                } else if (is_equ_e(g2_sub->lhs)) {
                    //do equals..
                }
                if (is_var_e(g2_sub->rhs) && compare_atoms_e(g2_sub->rhs, sub_var)) {
                    //same, so sub
                    free_expr(g2_sub->rhs);
                   *g2_sub->rhs = copy_expr(sub_val);
                } else if (is_tuple_e(g2_sub->rhs)) {
                    tuple_eliminate(&g2_sub->rhs->e.t, sub_var, sub_val);
                } else if (is_equ_e(g2_sub->rhs)) {
                    //do equals..
                    //a = b = c
                    //a = b, a = c
                }
            }
        }
    }
}

//wrapper to enforce tuple type
void tuple_eliminate(tuple* tu, const expr* var, const expr* val) {
    rec_tuple_eliminate(&tu->n, var, val);
}

void rec_tuple_eliminate(and* nd, const expr* var, const expr* val) {
    if (is_var_e(nd->lhs) && compare_atoms_e(nd->lhs, var)) {
        free_expr(nd->lhs);
       *nd->lhs = copy_expr(val);
    }
    if (is_var_e(nd->rhs) && compare_atoms_e(nd->rhs, var)) {
        free_expr(nd->rhs);
       *nd->rhs = copy_expr(val);
    } else if (is_and_e(nd->rhs)) {
        rec_tuple_eliminate(&nd->rhs->e.n, var, val);
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

////old, may not need
void instantiate_a(atom* vr, atom* vl) {
    if (vr->type != a_var || vl->type != a_val) {
        printf("Error, instatiation failed. In instantiate_a\n");
        return;
    }
    free(vr->data.vr.symbol);
    vr->type = vl->type;
    vr->data = vl->data;
}

void instantiate_e(expr* vr, expr* vl) {
    if (vr->type != e_atom || vl->type != e_atom) {
        printf("Error, instatiation failed. In instantiate_e\n");
        return;
    }
    instantiate_a(&vr->e.a, &vl->e.a);
}

void rec_instantiate_e(expr* in, expr* vr, expr* vl) {
    if (is_var_e(in) && compare_atoms_e(in, vr)) {
        instantiate_e(in, vl);
    } else if (is_and_e(in)) {
        rec_instantiate_e(in->e.n.lhs, vr, vl);
        rec_instantiate_e(in->e.n.rhs, vr, vl);
    } else if (is_equ_e(in)) {
        rec_instantiate_e(in->e.e.lhs, vr, vl);
        rec_instantiate_e(in->e.e.rhs, vr, vl);
    } else if (is_fcall_e(in)) {
        for (int i = 0; i < vec_size(in->e.f.params); i++) {
            rec_instantiate_e(vec_at(in->e.f.params, i), vr, vl);
        }
    }// else val, skip
}
////end

void dump_frame(frame* frm) {
    printf("Frame has %i substitutions:\n", vec_size(frm->G));
    for (int g = 0; g < vec_size(frm->G); g++) {
        substitution* s = vec_at(frm->G, g);
        dump_expr(*s->lhs, true); printf(" = "); dump_expr(*s->rhs, false); nl;
    }
    printf("Sub calls:\n");
    for (int i = 0; i < vec_size(frm->next_calls); i++) {
        fcall* f = vec_at(frm->next_calls, i);
        printf("  "); dump_func_call(*f); nl;
    }
    printf("Result thus far: %s\n", outcome_to_string(&frm->last_result));
}

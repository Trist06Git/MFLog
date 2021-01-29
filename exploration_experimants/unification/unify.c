
#include <stdio.h>
#include <stdlib.h>

#include "unify.h"
#include "utils.h"
#include "debug_stuff.h"

void entry(vector* func_defs_cp) {
    choice_point* main_cp = get_cpoint_na(func_defs_cp, "main", 1);
    int call_sequ = 0;
    function* main_f = at(main_cp->functions, 0);
    frame* first_frame = init_frame(main_f, &call_sequ);
    //debug
    printf("Before unify:\n");
    dump_frame(first_frame);

    unify(first_frame, func_defs_cp, &call_sequ);

    printf("After unify:\n");
    dump_frame(first_frame);

    printf("\"main\" returned with answers:\n");
    for (int g = 0; g < size(first_frame->G); g++) {
        substitution* s = at(first_frame->G, g);
        for (int p = 0; p < size(main_f->params); p++) {
            atom* pr = at(main_f->params, p);
            expr pr_e = wrap_atom(*pr);
            if (is_var_e(&pr_e) && (compare_atoms_e(&pr_e, s->lhs) || compare_atoms_e(&pr_e, s->rhs))) {
                dump_expr(*s->lhs, true); printf(" = "); dump_expr(*s->rhs, false); nl;
            }
        }
    }
    /*
    for (int g = 0; g < size(first_frame->G); g++) {
        substitution* s = at(first_frame->G, g);
        if ((is_var_e(s->lhs) && compare_decomp_sequ(s->lhs, 0))
                ||
            (is_var_e(s->rhs) && compare_decomp_sequ(s->rhs, 0)))
        {
            dump_expr(*s->lhs, true); printf(" = "); dump_expr(*s->rhs, false); nl;
        }
    }*/
    
    free_frame(first_frame);
}

outcome call(frame_call* fr_c, frame* prev_frm, vector* func_defs_cp, int* call_sequ) {
    fcall fc = fr_c->fc;
    choice_point* f_cp = get_cpoint_na(func_defs_cp, fc.name, size(fc.params));
    frame* next_frm = init_frame(at(f_cp->functions, 0), call_sequ);
    outcome res = unify(next_frm, func_defs_cp, call_sequ);
    if (res == o_fail) return o_fail;
    //consolidate previous frame with next frame
    //G1 = G1 + G1 N G2, where N = intersect, G1 = prev_frm, G2 = next_frm
    for (int g2 = 0; g2 < size(next_frm->G); g2++) {
        substitution* s = at(next_frm->G, g2);
        if ((is_var_e(s->lhs) && compare_decomp_sequ(s->lhs, fr_c->call_sequence))
               ||
            (is_var_e(s->rhs) && compare_decomp_sequ(s->rhs, fr_c->call_sequence)))
        {
            substitution ret_s = copy_equ(s);
            push_back(prev_frm->G, &ret_s);
        }
    }
    free_frame(next_frm);
    return o_pass;//need a way of returning o_answers..
}

outcome unify(frame* frm, vector* func_defs_cp, int* call_sequ) {
    //first call funcs
    for (int i = 0; i < size(frm->next_calls); i++) {//change this to remove at back. latter
        frame_call* frc = at(frm->next_calls, i);
        if (call(frc, frm, func_defs_cp, call_sequ) == o_fail) {
            return o_fail;//should be retry
        }
    }
    //then swap
    for (int g = 0; g < size(frm->G); g++) {
        swap_substitution(at(frm->G, g));
    }
    //then eliminate
    eliminate(frm);
    //then delete 
    delete_g(frm);
    //then conflict
    outcome res = conflict(frm);
    return res;
}

frame* init_frame(function* f, int* call_sequ) {
    frame* frm = malloc(sizeof(frame));
    frm->G = new_vector(0, sizeof(substitution));
    frm->last_result = o_pass;
    frm->next_calls = new_vector(0, sizeof(frame_call));

    //setup uniquely named parameter variables, so as not to
    //clash with the caller
    for (int p = 0; p < size(f->params); p++) {
        atom param = copy_atom(at(f->params, p));
        substitution call_binds;
        call_binds.lhs = malloc(sizeof(expr));
        call_binds.rhs = malloc(sizeof(expr));
       *call_binds.lhs = make_var_e(decomp_name(call_sequ, &p));
       *call_binds.rhs = wrap_atom(param);
        push_back(frm->G, &call_binds);
    }
    (*call_sequ)++;
    //when calling sub functions, they must have different vars
    //from this functions return vars

    //add all of f's equations
    rec_add_expr(frm, &f->e, call_sequ);

    return frm;
}

void free_frame(frame* frm) {
    for (int g = 0; g < size(frm->G); g++) {
        free_equ(at(frm->G, g));
    }
    free_vector(frm->G);
    for (int i = 0; i < size(frm->next_calls); i++) {
        frame_call* fc = at(frm->next_calls, i);
        free_fcall(&fc->fc);
    }//a frame may have failed early, so all fcall may not have been removed
    free_vector(frm->next_calls);
    free(frm);
}

//does copying... because we need to modify the substitutions.
void rec_add_expr(frame* frm, expr* ex, int* call_sequ) {
    if (is_equ_e(ex)) {//only supports x = y for now, no x = z = y...
        substitution s = copy_equ(&ex->e.e);
        push_back(frm->G, &s);
    } else if (is_atom_e(ex)) {
        substitution s;
        s.lhs = malloc(sizeof(expr));
        s.rhs = malloc(sizeof(expr));
       *s.lhs = make_query(&ex->e.a);
       *s.rhs = copy_expr(&debug_dummy_var);
    } else if (is_fcall_e(ex)) {
        vector* params = ex->e.f.params;
        for (int i = 0; i < size(params); i++) {
            substitution s;
            s.lhs = malloc(sizeof(expr));
            s.rhs = malloc(sizeof(expr));
           *s.lhs = make_var_e(decomp_name(call_sequ, &i));
           *s.rhs = copy_expr(at(params, i));
            push_back(frm->G, &s);
        }
        frame_call f;
        f.call_sequence = *call_sequ;
        f.fc = ex->e.f;
        push_back(frm->next_calls, &f);
        *call_sequ++;
    } else if (is_and_e(ex)) {
        rec_add_expr(frm, ex->e.n.lhs, call_sequ);
        rec_add_expr(frm, ex->e.n.rhs, call_sequ);
    }
}

//only swap if var is on lhs, skip otherwise
void swap_substitution(substitution* sub) {
    if (!is_var_e(sub->lhs) && is_var_e(sub->rhs)) {
        expr* temp = sub->lhs;
        sub->lhs = sub->rhs;
        sub->rhs = temp;
    }
}

void delete_g(frame* frm) {
    for (int g = 0; g < size(frm->G); g++) {
        substitution* sub = at(frm->G, g);
        if (compare_atoms_e(sub->lhs, sub->rhs)) {
            free_equ(sub);
            remove_at(frm->G, g); g--;
        }
    }
}

//not actually needed anymore
void decompose(frame* frm) {
    for (int g = 0; g < size(frm->G); g++) {
        substitution* sub = at(frm->G, g);
        if (is_fcall_e(sub->lhs) && is_fcall_e(sub->rhs)) {
            fcall* lhs_f = &sub->lhs->e.f;
            fcall* rhs_f = &sub->rhs->e.f;
        }
    }
}

void old_decompose(frame* frm) {
    for (int g = 0; g < size(frm->G); g++) {
        substitution* sub = at(frm->G, g);
        if (is_fcall_e(sub->lhs) && is_fcall_e(sub->rhs)) {
            fcall* lhs_f = &sub->lhs->e.f;
            fcall* rhs_f = &sub->rhs->e.f;
            for (int i = 0; i < size(lhs_f->params); i++) {
                substitution new_sub;
                *new_sub.lhs = copy_expr(at(lhs_f->params, i));
                *new_sub.rhs = copy_expr(at(rhs_f->params, i));
                push_back(frm->G, &new_sub);
            }
            push_back(frm->next_calls, rhs_f);
            remove_at(frm->G, g); g--;
        }
    }
}

outcome conflict(frame* frm) {
    for (int g = 0; g < size(frm->G); g++) {
        substitution* sub = at(frm->G, g);
        if (is_val_e(sub->lhs) && is_val_e(sub->rhs)) {
            if (!compare_atoms_e(sub->lhs, sub->rhs)) {
                frm->last_result = o_fail;
                return o_fail;
            }
        }
    }
    return o_pass;
}

void eliminate(frame* frm) {
    for (int g = 0; g < size(frm->G); g++) {
        substitution* sub = at(frm->G, g);
        expr* sub_var = sub->lhs;
        expr* sub_val = sub->rhs;
        if (is_var_e(sub_var)) {
            //eliminate in G
            for (int g2 = 0; g2 < size(frm->G); g2++) {
                if (g2 == g) continue;//skip self
                substitution* g2_sub = at(frm->G, g2);
                if (is_var_e(g2_sub->lhs) && compare_atoms_e(g2_sub->lhs, sub_var)) {
                    //they are the same, so substitute
                    free_expr(g2_sub->lhs);
                    *g2_sub->lhs = copy_expr(sub_val);
                }
                if (is_var_e(g2_sub->rhs) && compare_atoms_e(g2_sub->rhs, sub_var)) {
                    //same, so sub
                    free_expr(g2_sub->rhs);
                    *g2_sub->rhs = copy_expr(sub_val);
                }
            }
        }
    }
}

outcome query(expr* ex) {
    return o_pass;//not done yet
}

char* outcome_to_string(const outcome* o) {
    switch (*o) {
        case o_answers : return answ_string ; break;
        case o_pass    : return pass_string ; break;
        case o_fail    : return fail_string ; break;
        default        : return random_error; break;
    }
}

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
        for (int i = 0; i < size(in->e.f.params); i++) {
            rec_instantiate_e(at(in->e.f.params, i), vr, vl);
        }
    }// else val, skip
}

void dump_frame(frame* frm) {
    printf("Frame has %i substitutions:\n", size(frm->G));
    for (int g = 0; g < size(frm->G); g++) {
        substitution* s = at(frm->G, g);
        dump_expr(*s->lhs, true); printf(" = "); dump_expr(*s->rhs, false); nl;
    }
    printf("Still to call:\n");
    for (int i = 0; i < size(frm->next_calls); i++) {
        fcall* f = at(frm->next_calls, i);
        printf("  "); dump_func_call(*f); nl;
    }
    printf("Result thus far: %s\n", outcome_to_string(&frm->last_result));
}

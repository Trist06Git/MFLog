
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "unify.h"
#include "utils.h"
#include "debug_stuff.h"

extern fcall fc_main;

//#define UNIFY_DEBUG

void entry(vector* func_defs_cp, vector* globals) {
    choice_point* main_cp = get_cpoint_na(func_defs_cp, "main", 1);
    if (main_cp == NULL) {
        printf("Error. Could not find a main with arity 1.\n  >1 arity not yet supported.\n");
        return;
    }
    int call_sequ = 0;
    function* main_f = vec_at(main_cp->functions, 0);
    
    frame dummy_prev;
    dummy_prev.changes = false;
    dummy_prev.fname = "";
    dummy_prev.G = new_vector(0, 1);
    dummy_prev.next_calls = new_vector(0, 1);
    frame* first_frame = init_frame(main_f, &fc_main, &dummy_prev, globals, &call_sequ);

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
    } else if (entry_res == o_undet) {
        printf("\"main\" returned but its results could not be determined.\n");
    } else if (entry_res == o_fail) {
        printf("\"main\" returned with a failure.\n");
    }

    free_frame(first_frame);
}

bool is_list_instantiated_e(expr* e) {
    list* lst = &e->e.a.data.vl.v.l;
    if (!lst->has_vars) return true;
    mf_array* arr = lst->lst;
    for (int i = 0; i < mfa_card(arr); i++) {
        expr* ei = mfa_at(arr, i);
        if (is_var_e(ei)) return false;
    }
    lst->has_vars = false;
    return true;
}

#define CONSOLIDATE_DEBUG
//consolidate previous frame with next frame
//G1 = G1 + G1 N G2, where N = intersect, G1 = prev_frm, G2 = next_frm
outcome consolidate_frames(frame* prev_frm, frame* next_frm, int call_sequ) {
    //vector* prevs_subs = new_vector(0, sizeof(substitution));
    //vector* nexts_subs = new_vector(0, sizeof(substitution));
    for (int g2 = 0; g2 < vec_size(next_frm->G); g2++) {
        substitution* s = vec_at(next_frm->G, g2);
        //first check for uninstantiated list
        if (is_list_e(s->lhs) && !is_list_instantiated_e(s->lhs)) {
            printf("Error. In %s, list is not fully instantiated and contains variables.\n", next_frm->fname);
            //maybe print var/list name also?
            return o_fail;
        }
        if (is_list_e(s->rhs) && !is_list_instantiated_e(s->rhs)) {
            printf("Error. In %s, list is not fully instantiated and contains variables.\n", next_frm->fname);
            return o_fail;
        }
        if ((is_var_e(s->lhs) && compare_decomp_sequ(s->lhs, call_sequ))
                ||
            (is_var_e(s->rhs) && compare_decomp_sequ(s->rhs, call_sequ)))
        {
            substitution ret_s = copy_equ(s);
            //vec_push_back(prevs_subs, &ret_s);
            //substitution in_s = copy_equ(get_sub_frm(prev_frm, s->lhs));
            //vec_push_back(nexts_subs, &in_s);


            vec_push_back(prev_frm->G, &ret_s);
            //return o_pass;
        }
    }
    return o_pass;
    /*
    for (int i = 0; i < vec_size(nexts_subs); i++) {
        vec_push_back(next_frm->G, vec_at(nexts_subs, i));
    }
    eliminate(next_frm);
    outcome res = conflict(next_frm);
#ifdef CONSOLIDATE_DEBUG
    printf("DEBUG :: %s : After consolidation:\n", next_frm->fname);
    dump_frame(next_frm);
#endif

    if (res == o_fail) {
        for (int i = 0; i < vec_size(nexts_subs); i++) {
            //free_expr(vec_at(nexts_subs, i));
            free_expr(vec_at(prevs_subs, i));//risky
        }
        free_vector(nexts_subs);
        free_vector(prevs_subs);
        return o_fail;
    } else {
        for (int i = 0; i < vec_size(prevs_subs); i++) {
            vec_push_back(prev_frm->G, vec_at(prevs_subs, i));
            //free_expr(vec_at(nexts_subs, i));
        }
        free_vector(nexts_subs);
        free_vector(prevs_subs);
        return o_pass;
    }*/
}

substitution make_uni_sub(int call_sequ, int param_sequ) {
    substitution s;
    s.lhs = malloc(sizeof(expr));
   *s.lhs = make_var_e(decomp_name(&call_sequ, &param_sequ));
    s.rhs = malloc(sizeof(expr));
    return s;
}

//only need name/arity and passed params
outcome call_builtin(fcall* fc, frame* frm, int call_sequ) {
    if (strcmp(fc->name, "integer") == 0) {
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
            }
        } else if (is_val_e(param) && param->e.a.data.vl.type == v_int) {
            if (fc->res_set == rs_first && param->e.a.data.vl.v.i != 0) return o_fail;
            return o_pass;
        } else {
            return o_fail;
        }
    } else if (strcmp(fc->name, "less_than") == 0 ||
               strcmp(fc->name, "greater_than") == 0) {
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
            }
        } else {
            printf("Info. Not all answer sets have been implemented for \"<\" yet sorry.\n");
        }
    } else if (strcmp(fc->name, "print") == 0) {
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
            //printf("\n");// <<----- to be replaced with nl.
            return o_pass;
        }
    } else if (strcmp(fc->name, "nl") == 0) {
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
    } else {
        printf("Info. Sorry, builtin function \"%s\" is not yet implemented.\n", fc->name);
        return o_pass;
    }
    return o_fail;
}

outcome call(frame_call* fr_c, frame* prev_frm, vector* func_defs_cp, vector* globals, int* call_sequ) {
#ifdef UNIFY_DEBUG
    printf("DEBUG :: Calling frame for \"%s\"\n", fr_c->fc.name);
#endif
    fcall fc = fr_c->fc;
    choice_point* f_cp = get_cpoint_na(func_defs_cp, fc.name, vec_size(fc.params));
    if (fc.type == f_builtin) {
        return call_builtin(&fc, prev_frm, fr_c->call_sequence);//only first answer for now
    } else if (f_cp == NULL) {
        printf("Error. Could not find function \"%s\" with arity %i\n", fc.name, vec_size(fc.params));
        return o_fail;
    }

    //only get the first answer
    if (fc.res_set == rs_first) {
        function* first_cp = vec_at(f_cp->functions, 0);
        int fs_sequ = fr_c->call_sequence;
        frame* next_frm = init_frame(first_cp, &fc, prev_frm, globals, &fs_sequ);
        outcome res = unify(next_frm, func_defs_cp, globals, call_sequ);
        if (res == o_fail) return o_fail;

        res = consolidate_frames(prev_frm, next_frm, fr_c->call_sequence);
        free_frame(next_frm);
        return res;

    } else if (fc.res_set == rs_one) {//retry enabled
        
        for (int i = 0; i < vec_size(f_cp->functions); i++) {
            function* f = vec_at(f_cp->functions, i);
            int fs_sequ = fr_c->call_sequence;
            frame* next_frm = init_frame(f, &fc, prev_frm, globals, &fs_sequ);
            outcome res = unify(next_frm, func_defs_cp, globals, call_sequ);
            if (res == o_pass) {
                res = consolidate_frames(prev_frm, next_frm, fr_c->call_sequence);//what about returned res?
                free_frame(next_frm);
                if (res != o_fail) {
                    return res;
                }
                //return res;
            } else if (res == o_fail) {
#ifdef UNIFY_DEBUG
                printf("::::DEBUG: failed, retrying.\n");
#endif
                free_frame(next_frm);
            } else {
                printf("@@@@@@@@@not fail or pass after unify in call()\n");
            }
        }
#ifdef UNIFY_DEBUG
        printf("::::DEBUG: no answers.\n");
#endif
        return o_fail;
    } else {
        printf("Info. Only the first or existence choice point answer is currently implemented\n");
        printf("for function: %s/%i\n", fc.name, vec_size(fc.params));
        return o_pass;
    }
    //return o_pass;//need a way of returning o_answers..
}

outcome unify_substitutions(frame* frm) {
    //reset changes
    frm->changes = false;
    //Swap has been moved into eliminate
    //then eliminate
    eliminate(frm);
    //decompose tuples
    decompose(frm);
    //then delete 
    delete_g(frm);
    //then conflict
    outcome res = conflict(frm);
    if (res == o_fail) return o_fail;
#ifdef UNIFY_DEBUG
    printf("DEBUG :: %s : After unify:\n", frm->fname);
    dump_frame(frm);
#endif
    return o_pass;
}

outcome unify(frame* frm, vector* func_defs_cp, vector* globals, int* call_sequ) {
#ifdef UNIFY_DEBUG
    printf("DEBUG :: %s : Before unify:\n", frm->fname);
    dump_frame(frm);
#endif
    //initial unify
    bool last_changes = frm->changes;
    outcome res = unify_substitutions(frm);
    frm->changes |= last_changes;//this needs to be done because we are subbing 2wice per unify
    if (res == o_fail) return o_fail;

    bool undetermined = false;
    //call funcs. g, f, etc
    for (int i = 0; i < vec_size(frm->next_calls); i++) {
        frame_call* frc = vec_at(frm->next_calls, i);
        if (frc->undet) {
            if (!frm->changes) {
                //this fcall was undetermined and nothing has changed
#ifdef UNIFY_DEBUG
                printf("DEBUG :: %s : Unification is undeterminable at this point.\n", frm->fname);
#endif
                return o_undet;
            }
            res = call(frc, frm, func_defs_cp, globals, call_sequ);
            if (res == o_fail) {
                return o_fail;
            } else if (res == o_undet) {
                undetermined = true;
            } else if (res == o_pass) {
                frc->undet = false;
            }
        }
    }
#ifdef UNIFY_DEBUG
    if (vec_size(frm->next_calls) > 0) {
        printf("DEBUG :: %s : After calls:\n", frm->fname);
        dump_frame(frm);
    }
#endif
    
    //unify this frames scope
    res = unify_substitutions(frm);
    if (res == o_fail) return o_fail;

    if (undetermined) {
#ifdef UNIFY_DEBUG
        printf("DEBUG :: %s : Unification still has unbound variables, trying again\n", frm->fname);
#endif
        res = unify(frm, func_defs_cp, globals, call_sequ);
    }

    return res;
}

frame* init_frame(function* f, fcall* fc, frame* prev_frm, vector* globals, int* call_sequ) {
    frame* frm = malloc(sizeof(frame));
    frm->G = new_vector(0, sizeof(substitution));
    frm->last_result = o_pass;
    frm->next_calls = new_vector(0, sizeof(frame_call));
    frm->fname = fc->name;
    frm->changes = true;

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
    //fetch the already bound vals for passed vars
    for (int g = 0; g < vec_size(prev_frm->G); g++) {
        substitution* g_sub = vec_at(prev_frm->G, g);
        if (is_val_e(g_sub->rhs) && compare_decomp_sequ(g_sub->lhs, *call_sequ)) {
            substitution s;
            s.lhs = malloc(sizeof(expr));
            s.rhs = malloc(sizeof(expr));
           *s.lhs = copy_expr(g_sub->lhs);
           *s.rhs = copy_expr(g_sub->rhs);
            vec_push_back(frm->G, &s);
        }
    }
    
    int list_no = 0;
    for (int p = 0; p < vec_size(fc->params); p++) {
        expr* p_ex = vec_at(fc->params, p);
        
        //uniquely name list vars
        ////////this may need more work
        if(is_list_e(p_ex) && !is_list_instantiated_e(p_ex)) {
            int arg_no = 0;
            list* lst = &p_ex->e.a.data.vl.v.l;
            for (int i = 0; i < mfa_card(lst->lst); i++) {
                expr* ei = mfa_at(lst->lst, i);
                if (is_var_e(ei)) {
                    //free ei
                   *ei = make_var_e(anon_list_name(list_prefix, *call_sequ, list_no, arg_no));
                    arg_no++;
                }
            }
            list_no++;
        }
        //add passed params
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
        //never gets here because of explication
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
        f.undet = true;
        f.call_sequence = *call_sequ;
        f.fc = copy_fcall(&e->e.f);
        vec_push_back(frm->next_calls, &f);
        (*call_sequ)++;
    }
}

//var is a variable
//returned NULL = not found
substitution* get_sub_frm(frame* frm, expr* var) {
    if (!is_var_e(var)) return NULL;
    for (int g = 0; g < vec_size(frm->G); g++) {
        substitution* sub = vec_at(frm->G, g);
        if (compare_atoms_e(var, sub->lhs)) return sub;
    }
    return NULL;
}

//decompose tuples\lists
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
        } else if (is_list_e(sub->lhs) &&
                   is_list_e(sub->rhs)) {
            //i think this is old.
        }
    }
}

//only swap if var is on rhs, skip otherwise
void swap_substitution(substitution* sub) {
    if (!is_var_e(sub->lhs) && is_var_e(sub->rhs)) {
        expr* temp = sub->lhs;
        sub->lhs = sub->rhs;
        sub->rhs = temp;
    } else if (is_list_e(sub->lhs) && is_list_instantiated_e(sub->lhs)
                  &&
               is_list_e(sub->rhs) && !is_list_instantiated_e(sub->rhs)) {
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
                    frm->changes = true;
                } else if (is_tuple_e(g2_sub->lhs)) {
                    tuple_eliminate(&g2_sub->lhs->e.t, sub_var, sub_val, frm);
                    //changes
                } else if (is_list_e(g2_sub->lhs)) {
                    list_eliminate(&g2_sub->lhs->e.a.data.vl.v.l, sub_var, sub_val, frm);
                    //changes
                }

                if (is_var_e(g2_sub->rhs) && compare_atoms_e(g2_sub->rhs, sub_var)) {
                    //same, so sub
                    free_expr(g2_sub->rhs);
                   *g2_sub->rhs = copy_expr(sub_val);
                    frm->changes = true;
                } else if (is_tuple_e(g2_sub->rhs)) {
                    tuple_eliminate(&g2_sub->rhs->e.t, sub_var, sub_val, frm);
                    //changes
                } else if (is_list_e(g2_sub->rhs)) {
                    list_eliminate(&g2_sub->rhs->e.a.data.vl.v.l, sub_var, sub_val, frm);
                    //changes
                } else if (is_equ_e(g2_sub->rhs) || is_equ_chain_e(g2_sub->rhs)) {
                    printf("ERROR in eliminate() .. is_equ_*e()");
                }
            }
        } else if (is_list_e(sub_var) &&
                  !is_list_instantiated_e(sub_var) &&
                   is_list_e(sub_val)) 
        {
            double_list_eliminate(&sub_var->e.a.data.vl.v.l, &sub_val->e.a.data.vl.v.l, frm);
            //changes
        }
    }
}

void double_list_eliminate(list* lst1, list* lst2, frame* frm) {
    if (lst1->lst == NULL || lst2->lst == NULL) return;
    if (mfa_card(lst1->lst) != mfa_card(lst2->lst)) return;
    for (int i = 0; i < mfa_card(lst1->lst); i++) {
        expr* lhs = mfa_at(lst1->lst, i);
        expr* rhs = mfa_at(lst2->lst, i);
        if (is_var_e(lhs) || is_var_e(rhs)) {
            substitution s;
            s.lhs = malloc(sizeof(expr));
           *s.lhs = copy_expr(lhs);
            s.rhs = malloc(sizeof(expr));
           *s.rhs = copy_expr(rhs);
            vec_push_back(frm->G, &s);
            if (is_var_e(lhs) && !is_var_e(rhs)) {
                //free lhs
               *lhs = *rhs;
                frm->changes = true;
            } else if(!is_var_e(lhs) && is_var_e(rhs)) {
                //free rhs
               *rhs = *lhs;
                frm->changes = true;
            }
        }
    }
}

void list_eliminate(list* li, const expr* var, const expr* val, frame* frm) {
    if (li->lst == NULL) return;
    for (int i = 0; i < mfa_card(li->lst); i++) {
        expr* ei = mfa_at(li->lst, i);
        if (is_var_e(ei) && compare_atoms_e(ei, var)) {
            //expr* eold;
            //*eold = *ei;
            //free_expr(eold);
           *ei = copy_expr(val);
            frm->changes = true;
        }
    }
}

void tuple_eliminate(tuple* tu, const expr* var, const expr* val, frame* frm) {
    for (int i = 0; i < vec_size(tu->n.ands); i++) {
        expr* ti = vec_at(tu->n.ands, i);
        if (is_var_e(ti) && compare_atoms_e(ti, var)) {
            //free_expr(ti);//this is bad, as the expr lives in the vector and cant be freed, but what about its sub elements?
           *ti = copy_expr(val);
            frm->changes = true;
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
        case o_undet   : return undet_string ;
        default        : return random_error ;
    }
}

void dump_frame(frame* frm) {
    printf("Frame has %i substitutions and %s changed\n", vec_size(frm->G), frm->changes ? "just" : "not");
    for (int g = 0; g < vec_size(frm->G); g++) {
        substitution* s = vec_at(frm->G, g);
        dump_expr(*s->lhs, true); printf(" = "); dump_expr(*s->rhs, false); nl;
    }
    printf("Sub calls:\n");
    for (int i = 0; i < vec_size(frm->next_calls); i++) {
        fcall* f = vec_at(frm->next_calls, i);
        printf("  "); dump_func_call(*f, true); nl;
    }
    //printf("Result thus far: %s\n", outcome_to_string(&frm->last_result));
}

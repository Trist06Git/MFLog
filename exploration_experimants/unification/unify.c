
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "unify.h"
#include "builtin_calls.h"
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

    int global_call_sequ = 1;
    function* main_f = vec_at(main_cp->functions, 0);
    frame* first_frame = init_frame(main_f, &fc_main, NULL, globals, &global_call_sequ, 0);
    outcome entry_res = unify(first_frame, func_defs_cp, globals, &global_call_sequ);

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
outcome consolidate_frames_old(frame* prev_frm, frame* next_frm, int call_sequ) {
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
            vec_push_back(prev_frm->G, &ret_s);
        }
    }
    return o_pass;
}

//no list checking for now..
void consolidate_frames(frame* prev_frm, frame* next_frm) {
    for (int g = 0; g < vec_size(next_frm->G); g++) {
        substitution* g_sub = vec_at(next_frm->G, g);
        bool contains = false;
        for (int g2 = 0; g2 < vec_size(prev_frm->G); g2++) {
            substitution* g2_sub = vec_at(prev_frm->G, g2);
            if (compare_equs_e(g_sub, g2_sub)) {
                contains = true;
                break;
            }
        }
        if (!contains) {
            substitution new_sub = copy_equ(g_sub);
            vec_push_back(prev_frm->G, &new_sub);
        }
    }
}

outcome call(frame_call* frc, frame* prev_frm, frame* next_frm, vector* func_defs_cp, vector* globals, int* call_sequ) {
#ifdef UNIFY_DEBUG
    printf("DEBUG :: Calling frame for \"%s\" at choice point %i\n", frc->fc.name, frc->cp_count);
#endif
    fcall fc = frc->fc;
    //get choice points
    choice_point* f_cp = get_cpoint_na(func_defs_cp, fc.name, vec_size(fc.params));
    //just double check things
    if ((frc->cp_count < 0 && frc->fc.type != f_builtin) ||
        (frc->cp_count >= vec_size(f_cp->functions) && frc->fc.type != f_builtin))
    {
        printf("INTERNAL :: Error, invalid choice point for function %s : %i\n", frc->fc.name, frc->cp_count);
        exit(-1);
    }

    if (fc.type == f_builtin) {
        //call builtin
#ifdef UNIFY_DEBUG
        printf("DEBUG :: Calling builtin \"%s\"\n", fc.name);
#endif
        frame dummy_frame;
        dummy_frame.G = new_vector(0, 1);
        dummy_frame.G_clean = new_vector(0, 1);
        dummy_frame.next_calls = new_vector(0, 1);
       *next_frm = dummy_frame;/// <-- pretty bad..
        return call_builtin(&fc, prev_frm, frc->call_sequence);//only first answer for now
    } else if (f_cp == NULL) {
        //func not found
        printf("Error. Could not find function \"%s\" with arity %i\n", fc.name, vec_size(fc.params));
        return o_fail;
    }

    if (next_frm == NULL) printf("WARN :: unexpected NULL next_frm in call()\n");
    function* f = vec_at(f_cp->functions, frc->cp_count);
    frame* temp = init_frame(f, &fc, prev_frm, globals, call_sequ, frc->call_sequence);
   *next_frm = *temp;
    
    return unify(next_frm, func_defs_cp, globals, call_sequ);
}

outcome unify_substitutions(frame* frm) {
    //reset changes
    frm->changes = false;
    //Swap has been moved into eliminate
    //then eliminate
    outcome res = eliminate(frm);
    if (res == o_fail) return o_fail;
    //decompose tuples
    decompose(frm);
    //then delete 
    delete_g(frm);
    //then conflict
    res = conflict(frm);
    
#ifdef UNIFY_DEBUG
    printf("DEBUG :: %s-%i : After unify:\n", frm->fname, frm->call_sequ);
    dump_frame(frm);
#endif
    if (res == o_fail) {
        return o_fail;
    } else {
        return o_pass;
    }
}

bool incr_cp_counts(frame* frm, vector* func_defs_cp) {
    for (int i = 0; i < vec_size(frm->next_calls); i++) {
        frame_call* fc = vec_at(frm->next_calls, i);
        //if (fc->fc.type == f_builtin) continue;
        int cp_count = get_cp_count(func_defs_cp, &fc->fc);
        if (cp_count == -1) {
            printf("EXPLOSION!!!\n");
            return false;
        }
        if (fc->cp_count < cp_count-1) {
            (fc->cp_count)++;
            return true;
        } else if (fc->cp_count == cp_count-1) {
            fc->cp_count = 0;//nullify ours
            for (int j = 0; j < i; j++) {
                frame_call* fcj = vec_at(frm->next_calls, j);
                fcj->cp_count = 0;
            }//nullify all others before us
            if (i < vec_size(frm->next_calls)-1) {
                //continue
            } else {
                return false;
            }
        }
    }
    return false;
}

void reset_determined(frame* frm) {
    for (int i = 0; i < vec_size(frm->next_calls); i++) {
        frame_call* fc = vec_at(frm->next_calls, i);
        fc->undet = true;
    }
}

outcome unify(frame* frm, vector* func_defs_cp, vector* globals, int* call_sequ) {
    //printf("DEBUG :: %s : Before first unify\n", frm->fname);
    //dump_frame(frm);
    outcome res = unify_substitutions(frm);
    if (res == o_fail) {
#ifdef UNIFY_DEBUG
        printf("DEBUG :: %s-%i : unify_substitutions() failed in unify()\n", frm->fname, frm->call_sequ);
#endif
        return o_fail;
    }
    //hack to keep the while loop, which i think is more clearer
    if (vec_size(frm->next_calls) > 0) {
        frame_call* fc = vec_at(frm->next_calls, 0);
        /*if (fc->fc.type != f_builtin)*/ fc->cp_count = -1;
    }
    //while there are still cp perms
    while (incr_cp_counts(frm, func_defs_cp)) {
        //call f, g
        for (int i = 0; i < vec_size(frm->next_calls); i++) {
            frame_call* frc = vec_at(frm->next_calls, i);
            frame* next_frm = malloc(sizeof(frame));
            res = call(frc, frm, next_frm, func_defs_cp, globals, call_sequ);
            if (res == o_fail) {
#ifdef UNIFY_DEBUG
                printf("DEBUG :: %s-%i : call() failed, trying next cp perm\n", frm->fname, frm->call_sequ);
#endif
                //try next perm
                free_frame(next_frm);
                //NOTE, may not need to clean G here.. seems faster if we do clean..
                free_G(frm->G);
                frm->G = duplicate_G(frm->G_clean);
                break;
            } else {//WARN, o_undet
                //try and consolidate
                consolidate_frames(frm, next_frm);
                free_frame(next_frm);
                res = unify_substitutions(frm);
                if (res == o_fail) {
#ifdef UNIFY_DEBUG
                    printf("DEBUG :: %s-%i : consolidation of successful call failed,\ncleaning G and trying next cp perm\n", frm->fname, frm->call_sequ);
#endif
                    //clean G, try next perm
                    free_G(frm->G);
                    frm->G = duplicate_G(frm->G_clean);
                    break;
                } else {
#ifdef UNIFY_DEBUG
                    printf("DEBUG :: %s-%i : call and consolidation of %s %s\n", frm->fname, frm->call_sequ, frc->fc.name, outcome_to_string(&res));
#endif
                }
            }
        }
        if (res != o_fail) {
#ifdef UNIFY_DEBUG
            printf("DEBUG :: %s-%i : Found answer, returning success\n", frm->fname, frm->call_sequ);
#endif
            break;//found an answer
        } else {
#ifdef UNIFY_DEBUG
            printf("DEBIG :: incrementing cp_counts\n");
#endif
        }
    }
    return res;
}

//function and globals Can be NULL inwhich their exprs will not be added to the frame
//this is to be used for builtins 
frame* init_frame(function* f, fcall* fc, frame* prev_frm, vector* globals, int* global_call_sequ, int this_call_sequ) {
#ifdef UNIFY_DEBUG
    printf("DEBUG :: Initialising frame for %s inside %s's frame\n", fc->name, prev_frm==NULL ? "BEGIN" : prev_frm->fname);
#endif
    frame* frm = malloc(sizeof(frame));
    frm->last_result = o_pass;
    frm->next_calls = new_vector(0, sizeof(frame_call));
    frm->fname = fc->name;
    frm->changes = true;
    frm->call_sequ = this_call_sequ;

    //copy prev_frame and add to it
    if (prev_frm != NULL) {
        frm->G = duplicate_G(prev_frm->G);
    } else {
        frm->G = new_vector(0, sizeof(substitution));
    }
    
    //setup uniquely named parameter variables, so as not to
    //clash with the caller
    if (f != NULL) for (int p = 0; p < vec_size(f->params); p++) {
        atom param = copy_atom(vec_at(f->params, p));
        substitution call_binds;
        call_binds.lhs = malloc(sizeof(expr));
        call_binds.rhs = malloc(sizeof(expr));
       *call_binds.lhs = make_var_e(decomp_name(&this_call_sequ, &p));
       *call_binds.rhs = wrap_atom(param);
        if (is_var_e(call_binds.rhs)) {
            prepend_unique_var_e(call_binds.rhs, frm->call_sequ);
        }
        vec_push_back(frm->G, &call_binds);
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
                   *ei = make_var_e(anon_list_name(list_prefix, *global_call_sequ, list_no, arg_no));
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
           *call_bind.lhs = make_var_e(decomp_name(&this_call_sequ, &p));
           *call_bind.rhs = passed;
            if (is_var_e(call_bind.rhs)) {
                prepend_unique_var_e(call_bind.rhs, frm->call_sequ);
            }
            vec_push_back(frm->G, &call_bind);
        }
        
    }

    //add all of f's equations
    if (f != NULL) {
        add_frame_exprs(frm, &f->e, global_call_sequ);
    }
    //add global scope equations
    if (globals != NULL && prev_frm == NULL) {
        for (int i = 0; i < vec_size(globals); i++) {
            expr gls;
            gls.type = e_and;
            gls.e.n.ands = globals;
            add_frame_exprs(frm, &gls, global_call_sequ);
        }
    }
    frm->G_clean = duplicate_G(frm->G);
    return frm;
}

void free_frame(frame* frm) {
    //for (int g = 0; g < vec_size(frm->G); g++) {
    //    free_equ(vec_at(frm->G, g));
    //}
    //free_vector(frm->G);
    free_G(frm->G);
    free_G(frm->G_clean);
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
        //prepend this frames call_sequ to make the vars unique
        if (is_var_e(s.lhs)) {
            prepend_unique_var_e(s.lhs, frm->call_sequ);
        }
        if (is_var_e(s.rhs)) {
            prepend_unique_var_e(s.rhs, frm->call_sequ);
        }
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
            if (is_var_e(s.rhs)) {
                prepend_unique_var_e(s.rhs, frm->call_sequ);
            }
            vec_push_back(frm->G, &s);
        }
        frame_call f;
        f.undet = true;
        f.call_sequence = *call_sequ;
        f.cp_count = 0;
        f.fc = copy_fcall(&e->e.f);
        vec_push_back(frm->next_calls, &f);
        (*call_sequ)++;
    }
}

void prepend_unique_var_e(expr* vr, int call_sequ) {
    var* subject = &vr->e.a.data.vr;
    char* old = strdup(subject->symbol);
    subject->symbol = realloc(subject->symbol, sizeof(char) * (3+digits(call_sequ)+strlen(old)));//V_x_name
    sprintf(subject->symbol, "V_%i_%s", call_sequ, old);
    free(old);
}

vector* duplicate_G(vector* G) {
    vector* new_G = new_vector(vec_size(G), sizeof(substitution));
    for (int g = 0; g < vec_size(G); g++) {
        substitution s = copy_equ(vec_at(G, g));
        vec_push_back(new_G, &s);
    }
    return new_G;
}

//check this plz.
void free_G(vector* G) {
    for (int g = 0; g < vec_size(G); g++) {
        free_equ(vec_at(G, g));
    }
    free_vector(G);
}

//var is a variable
//returned NULL = not found
//WARNING, this only returns the first occurrence, not all.
substitution* get_sub_frm(frame* frm, expr* var) {
    if (!is_var_e(var)) return NULL;
    for (int g = 0; g < vec_size(frm->G); g++) {
        substitution* sub = vec_at(frm->G, g);
        if (compare_atoms_e(var, sub->lhs)) return sub;
    }
    return NULL;
}
//truly horrid..
substitution* get_sub_frm_i(frame* frm, int call_no, int var_no) {
    for (int g = 0; g < vec_size(frm->G); g++) {
        substitution* sub = vec_at(frm->G, g);
        if (is_var_e(sub->lhs)) {
            char* name = sub->lhs->e.a.data.vr.symbol;
            ///D_x_y
            if (strlen(name) < 3) return NULL;
            name += 2;//move to first num
            char* end = NULL;
            int this_call = strtol(name, &end, 10);
            name = end;
            end++;//move to second num
            char* end2 = NULL;
            int this_var = strtol(end, &end2, 10);

            if (this_call == call_no && this_var == var_no) {
                return sub;
            }

            //if (strlen(name) > 4 &&
            //    name[2] == call_no+48 &&
            //    name[4] ==  var_no+48
            //   ) {//D_x_y
            //    return sub;
            //}
        }
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
               for (int i = 0; i < vec_size(tupe_lhs); i++) {
                   expr* el_lhs = vec_at(tupe_lhs, i);
                   expr* el_rhs = vec_at(tupe_rhs, i);
                   substitution new_sub;
                   new_sub.lhs = malloc(sizeof(expr));
                   new_sub.rhs = malloc(sizeof(expr));
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
outcome eliminate(frame* frm) {
    for (int g = 0; g < vec_size(frm->G); g++) {
        substitution* sub = vec_at(frm->G, g);
        swap_substitution(sub);
        expr* sub_var = sub->lhs;
        expr* sub_val = sub->rhs;
        if (is_var_e(sub_var) && is_val_e(sub_val)) {
            //eliminate in G
            for (int g2 = 0; g2 < vec_size(frm->G); g2++) {
                if (g2 == g) continue;//skip self
                substitution* g2_sub = vec_at(frm->G, g2);
                


                if (is_var_e(g2_sub->lhs) && compare_atoms_e(g2_sub->lhs, sub_var)) {
                    //check for early conflict
                    if (is_val_e(sub_var) && is_val_e(g2_sub->rhs) && !compare_atoms_e(g2_sub->rhs, sub_var)) {
                        return o_fail;
                    }
                    //they are the same, so substitute
                    free_expr(g2_sub->lhs);
                   *g2_sub->lhs = copy_expr(sub_val);
                    if (compare_atoms_e(g2_sub->lhs, g2_sub->rhs)) {
                        vec_remove_at(frm->G, g2); g2--;
                    }
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
    return o_pass;
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
        if (is_var_e(&ex)) prepend_unique_var_e(&ex, 0);
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
    //printf("Frame has %i substitutions and %s changed\n", vec_size(frm->G), frm->changes ? "just" : "not");
    //printf("Frame has %i substitutions\n", vec_size(frm->G));
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

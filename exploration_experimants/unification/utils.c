
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

extern int errno;

//returns NULL if not found
choice_point* get_cpoint(vector* cps, function* f) {
    for (int i = 0; i < vec_size(cps); i++) {
        choice_point* cp = vec_at(cps, i);
        //get first
        function* this_f = vec_at(cp->functions, 0);
        if (compare_func_arity(this_f, f)) {
            return cp;
        }
    }
    return NULL;
}

//returns NULL if not found
choice_point* get_cpoint_na(vector* cps, const char* name, int arity) {
    for (int i = 0; i < vec_size(cps); i++) {
        choice_point* cp = vec_at(cps, i);
        function* first = vec_at(cp->functions, 0);
        if (strcmp(first->name, name) == 0 && vec_size(first->params) == arity) {
            return cp;
        }
    }
    return NULL;
}

bool func_point_exists(vector* cps, function* f) {
    for (int i = 0; i < vec_size(cps); i++) {
        if (func_def_exists(vec_at(cps, i), f)) {
            return true;
        }
    }
    return false;
}

bool func_def_exists(vector* defs, function* f) {
    //vectors need comparators..
    for (int i = 0; i < vec_size(defs); i++) {
        function* this_f = vec_at(defs, i);
        if (vec_size(f->params) != vec_size(this_f->params)) return false;
        if (compare_func_vals(f, this_f)) {
            return true;
        }
    }
    return false;
}

bool func_arity_exists(vector* defs, fcall* fc) {
    for (int i = 0; i < vec_size(defs); i++) {
        function* def = vec_at(defs, i);
        if (strcmp(fc->name, def->name) == 0
            &&
            vec_size(def->params) == vec_size(fc->params)
           ) {
            return true;
        }
    }
    return false;
}

bool compare_func_heads(function* f1, function* f2) {
    if (vec_size(f1->params) != vec_size(f2->params))
        return false;
    for (int i = 0; i < vec_size(f1->params); i++) {
        if(!compare_atoms_a(vec_at(f1->params, i), vec_at(f2->params, i))) {
            return false;
        }//else continue
    }
    return true;
}

bool compare_func_arity(function* f1, function* f2) {
    return vec_size(f1->params) == vec_size(f2->params)
           &&
           strcmp(f1->name, f2->name) == 0
           ;
}

bool same(atom* a, atom* b) {
    if (a->type == a_var && b->type == a_var) {
        return true;
    } else if (a->type == a_val
               &&
               b->type == a_val
               &&
               compare_atoms_a(a, b))
    {
        return true;
    }
    return false;
}

bool compare_func_vals(function* f1, function* f2) {
    if (strcmp(f1->name, f2->name) != 0)
        return false;
    if (vec_size(f1->params) != vec_size(f2->params))
        return false;
    for (int i = 0; i < vec_size(f1->params); i++) {
        if (!same(vec_at(f1->params, i), vec_at(f2->params, i))) {
            return false;
        }
    }
    return true;
}

function* get_fdef(vector* defs, char* name) {
    for (int i = 0; i < vec_size(defs); i++) {
        function* f = vec_at(defs, i);
        if (strcmp(name, f->name) == 0) {
            return f;
        }
    }
    return NULL;
}

function* get_fdef_defined(vector* defs, char* name) {
    for (int i = 0; i < vec_size(defs); i++) {
        function* f = vec_at(defs, i);
        if (f->fully_defined && strcmp(name, f->name) == 0) {
            return f;
        }
    }
    return NULL;
}

function* get_fdef_arity(vector* defs, char* name, int arity) {
    for (int i = 0; i < vec_size(defs); i++) {
        function* f = vec_at(defs, i);
        if (vec_size(f->params) != arity) return NULL;
        if (strcmp(name, f->name) == 0) {
            return f;
        }
    }
    return NULL;
}

int digits(int num) {
    int res = 0;
    do {res++;} while (num/=10);
    return res;
}

int digits_neg(int num) {
    int res = num >= 0 ? 0 : 1;
    do {res++;} while (num/=10);
    return res;    
}

//these need to be changed to unnamed symbol numbers
//avoiding expensive string comparison
char* unique_name_incr(int* n) {
    char* new_name = malloc(sizeof(char)*digits(*n)+1);
    sprintf(new_name, "%s%i", unique_prefix, *n);
    (*n)++;
    return new_name;
}
char* unique_name(int* n) {
    char* new_name = malloc(sizeof(char)*digits(*n)+1);
    sprintf(new_name, "%s%i", unique_prefix, *n);
    return new_name;
}

char* anon_name(const char* prefix, int a, int b) {
    char* new_name = malloc(sizeof(char)*(2+digits(a)+1+digits(b))+1);//T_+a+_+b+\0 = "T_1_2"
    sprintf(new_name, "%s%i_%i", prefix, a, b);
    return new_name;
}

char* anon_list_name(const char* prefix, int frm, int lst, int arg) {
    char* new_name = malloc(
        sizeof(char)*(
            strlen(prefix) +
            digits(frm) + 1 +
            digits(lst) + 1 +
            digits(arg) + 1
            )
    );//L_a_b_c\0
    sprintf(new_name, "%s%i_%i_%i", prefix, frm, lst, arg);
    return new_name;
}

//promise that vr is a variable
//needs overhaul
bool compare_list_sequ(expr* vr, int sequ) {
    if (!is_var_e(vr)) return false;
    int ds = digits(sequ);
    char* s_sequ = malloc(sizeof(char)*(ds+1));
    sprintf(s_sequ, "%i", sequ);
    
    char* subject = vr->e.a.data.vr.symbol;
    if (strlen(subject) < 2+ds || subject[0] != 'L') return false;
    subject += 2;//move past the "L_"
    for (int i = 0; i < strlen(s_sequ) && *subject != '0'; i++, subject++) {
        if (subject[0] != s_sequ[0]) return false;
    }

    free(s_sequ);
    return true;
}

////should probably change this around to be D_func_arity
//like in the prolog api, eg incr/2
char* decomp_name_incr(int* arity, int* func) {
    char* new_name = anon_name(decompose_prefix, *arity, *func);
    (*arity)++;
    return new_name;
}
char* decomp_name(int* arity, int* func) {
    return anon_name(decompose_prefix, *arity, *func);
}

//needs overhaul
bool compare_decomp_sequ(expr* vr, int sequ) {
    if (!is_var_e(vr)) return false;
    //itoa() is not a part of the c standard
    int ds = digits(sequ);
    char* s_sequ = malloc(sizeof(char)*(ds+1));
    sprintf(s_sequ, "%i", sequ);
    
    char* subject = vr->e.a.data.vr.symbol;
    if (strlen(subject) < 2+ds || subject[0] != 'D') return false;
    subject += 2;//move past the "D_"
    for (int i = 0; i < strlen(s_sequ) && *subject != '0'; i++, subject++) {
        if (subject[0] != s_sequ[0]) return false;
    }
    
    free(s_sequ);
    return true;
}

//Unfinished, not needed for now.
/*char* file_to_string(const char* filename_path) {
    if (filename_path == NULL) {
        printf("Error. Invalid filename. null pointer given.\n");
        return NULL;
    }
    FILE* file = fopen(filename_path, "r");
    if (file == NULL) {
        printf("Error. Could not open file %s:\n  %s\n", filename_path, strerror(errno));
        return NULL;
    }
    return NULL;
}*/

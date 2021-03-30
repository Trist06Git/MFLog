%define api.prefix {glob}
//%code provides {
//    #define YY_DECL int globlex (GLOBSTYPE *yylval, GLOBLTYPE *yylloc)
//    YY_DECL;
//}

%{
    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>
    #include "internal_struct.h"
    #include "generic_vector.h"
    #include "generic_map.h"
    #include "mlog_array.h"
    #include "utils.h"

    #include "debug_stuff.h"

    int globdebug = 1;
    extern int glob_line_number;

    bool glob_encountered_var = false;

    extern int flag;
    extern vector* func_defs;
    extern vector* global_defs;
    extern fcall fc_div;
    extern fcall fc_mul;
    extern fcall fc_plus;
    extern fcall fc_minus;
    extern fcall fc_print;
    extern fcall fc_nl;
    extern fcall fc_integer;
    extern fcall fc_lt;
    extern fcall fc_gt;
    extern fcall fc_cons;
    extern map* global_symbol_table;
    vector* glob_ps = NULL;
    mf_array* glob_ls = NULL;

    int globlex(void);
    void globerror(char*);
%}

%start Program

%union {
    char* string;
    int number;
    vector* vec;
    mf_array* arr;
    list u_lst;
    atom u_at;
    val u_val;
    var u_var;
    fcall u_fc;
    and u_and;
    equality u_eq;
    expr u_ex;
    function u_fn;
    enum rs_type u_rs;
}

%token <number> G_NUMBER
%token <string> G_WORD

%type <u_at> Atom
%type <u_val> Val
%type <u_var> Var
%type <u_fn> Func_head
%type <vec> Atom_params
%type <vec> Expr_params
%type <u_ex> Expr
%type <vec> Exprs
//%type <u_ex> Expr_eq
%type <arr> Expr_list
%type <u_lst> List
%type <u_fc> Fcall
%type <u_fc> Fcall_ans
%type <u_fc> Fbuiltin
%type <u_fc> Fbuiltin_ans
%type <u_fc> Builtin_func
%type <u_rs> Answer_count
%type <u_fc> Ocall
%type <u_fc> Operator
%type <u_fc> Cons_fcall
//%type <u_eq> Equation
%type <u_ex> Atom_left_equ

%type <vec> Equ_chain
%type <u_ex> Expr_eq_ch

%left G_MINUS
%left G_PLUS
%left G_MUL
%left G_DIV
%token G_LP_ROUND G_RP_ROUND
%token G_END
%left G_EQUAL
%left G_LESS_THAN
%left G_GREATER_THAN
%left G_AND
%token G_PRINT_F
%token G_NL_F
%token G_INTEGER_F
%token G_CONS_F
%token G_FST_ANS
%token G_ONE_ANS
%token G_ALL_ANS
%token G_LP_LIST G_RP_LIST
%token G_CONS_LIST
%token G_APP_LIST
%token G_AND_LIST
%token G_WILD_VAR

%%

Program : Func_def
        | Func_def Program
        | Fact     Program
        | Global   Program
        ;

Global : Atom_left_equ G_END {
    //expr ex = {.type = e_equ, .e.e = $1};
    vec_push_back(global_defs, &$1);
};

Func_def : Func_head Exprs G_END {
    //skip
};

Func_head
    : G_WORD G_LP_ROUND Atom_params G_RP_ROUND G_EQUAL {
        //skip
    }
    | G_WORD G_LP_ROUND G_RP_ROUND G_EQUAL {/*no params*/
        //skip
    }
    ;

Fact : G_WORD G_LP_ROUND Atom_params G_RP_ROUND G_END {
    //skip
}

Expr_list_skip
    : Expr_skip {
        //skip
    }
    | Expr_skip G_AND_LIST Expr_list_skip {
        //skip
    }
    ;

List_skip
    : G_LP_LIST Expr_list_skip G_RP_LIST {
        //skip
    }
    | G_LP_LIST G_RP_LIST {
        //skip
    }
    ;

Val_skip
    : List_skip {
        ///////truely horrific...
        //skip
    }
    | G_MINUS G_NUMBER {
        //skip
    }
    | G_NUMBER {
        //skip
    }
    ;

Var_skip : G_WORD {
    //skip
};

Atom_skip
    : Var_skip {
        //skip
    }
    | Val_skip {
        //skip
    }
    ;

Atom_params
    : Atom_skip {
        //skip
    }
    | Atom_skip G_AND Atom_params {
        //skip
    }
    ;

Expr_params
    : Expr_skip {
        //skip
    }
    | Expr_skip G_AND Expr_params {
        //skip
    }
    ;

Expr_skip
    : Fcall_ans     { /*whatever...*/ }
    | Fbuiltin_ans  { /*whatever...*/ }
    | Fcall         { /*whatever...*/ }
    | Fbuiltin      { /*whatever...*/ }
    | Atom_skip     { }
    | Ocall         { /*whatever...*/ }

Equ_chain_skip
    : Expr_skip {
        //skip
    }
    | Expr_skip G_EQUAL Equ_chain_skip {
        //skip
    }
    ;

Equ_chain
    : Expr {
        glob_ps = new_vector(1, sizeof(expr));
        vec_insert_at(glob_ps, 0, &$1);
        $$ = glob_ps;
        glob_ps = NULL;
    }
    | Expr G_EQUAL Equ_chain {
        if (glob_ps == NULL) glob_ps = new_vector(1, sizeof(expr));
        glob_ps =  $3;
        vec_insert_at(glob_ps, 0, &$1);
        $$ = glob_ps;
        glob_ps = NULL;
    }
    ;

Exprs
    : Expr_eq_ch {
        //skip
    }
    | Expr_eq_ch G_AND Exprs {
        //skip
    }
    ;

Expr : Fcall_ans     { expr ex; ex.type = e_fcall; ex.e.f  = $1; $$ = ex; }
     | Fbuiltin_ans  { expr ex; ex.type = e_fcall; ex.e.f  = $1; $$ = ex; }
     | Fcall         { expr ex; ex.type = e_fcall; ex.e.f  = $1; $$ = ex; }
     | Fbuiltin      { expr ex; ex.type = e_fcall; ex.e.f  = $1; $$ = ex; }
     | Atom          { expr ex; ex.type = e_atom;  ex.e.a  = $1; $$ = ex; }
     | Ocall         { expr ex; ex.type = e_fcall; ex.e.f  = $1; $$ = ex; }
     | Cons_fcall    { expr ex; ex.type = e_fcall; ex.e.f = $1; $$ = ex; }
     ;

Expr_list
    : Expr {
        glob_encountered_var = false;
        glob_ls = new_mfarray(sizeof(expr));
        mfa_push_front(glob_ls, &$1);
        glob_encountered_var |= is_var_e(&$1);
        $$ = glob_ls;
        glob_ls = NULL;
    }
    | Expr G_AND_LIST Expr_list {
        glob_ls = $3;
        mfa_push_front(glob_ls, &$1);
        glob_encountered_var |= is_var_e(&$1);
        $$ = glob_ls;
        glob_ls = NULL;
    }
    ;

List
    : G_LP_LIST Expr_list G_RP_LIST {
        //what if list has only vars?
        list l;
        l.has_vars = glob_encountered_var;
        l.type = v_int;
        l.lst = $2;
        $$ = l;
    }
    | G_LP_LIST G_RP_LIST {
        list l;
        l.has_vars = false;
        l.lst = NULL;//empty list
        l.type = v_notype;
        $$ = l;
    }
    ;

Expr_eq_ch : Equ_chain_skip {
    //skip
};

Fbuiltin : Builtin_func G_LP_ROUND Expr_params G_RP_ROUND {
    fcall fc = $1;
    fc.res_set = rs_one;
    fc.params = $3;
    $$ = fc;
};

Fbuiltin_ans : Answer_count Fbuiltin {
    fcall fc = $2;
    fc.res_set = $1;
    $$ = fc;
}

Fcall : G_WORD G_LP_ROUND Expr_params G_RP_ROUND {
    fcall fc;
    fc.res_set = rs_one;
    fc.name = $1;
    if (strcmp(fc.name, "plus") == 0) {
        fc.type = f_builtin;
    } else {
        fc.type = f_user;
    }
    fc.params = $3;
    $$ = fc;
};

Fcall_ans : Answer_count Fcall {
    fcall fc = $2;
    fc.res_set = $1;
    $$ = fc;
}

Cons_fcall
    : Expr G_CONS_LIST Expr {
        fcall cons;
        cons.name = strdup("cons");
        cons.type = f_builtin;
        cons.res_set = rs_one;
        cons.params = new_vector(3, sizeof(expr));
        vec_push_back(cons.params, &$1);
        vec_push_back(cons.params, &$3);
        $$ = cons;
    }
    | Cons_fcall G_CONS_LIST Expr {
        fcall cons = $1;
        vec_push_back(cons.params, &$3);
        $$ = cons;
    }
    ;

Val
    : G_NUMBER {
        val v;
        v.type = v_int;
        v.v.i = $1;
        $$ = v;
    }
    | G_MINUS G_NUMBER {
        val v;
        v.type = v_int;
        v.v.i = -$2;
        $$ = v;
    }
    | List {
        val v;
        v.type = v_list;
        v.v.l = $1;
        $$ = v;
    }
    ;

Var : G_WORD {
    var v;
    v.symbol.type = s_var;
    v.symbol.scope = 0;
    
    char* var_name = strdup($1);
    int* symbol_no = snd(global_symbol_table, &var_name);
    
    if (symbol_no == NULL) {
        //first encounter
        int i = map_size(global_symbol_table);
        map_add(global_symbol_table, &var_name, &i);
        v.symbol.num = i;
    } else {
        v.symbol.num = *symbol_no;
    }

    $$ = v;
};

Atom
    : Var {
        atom at;
        at.type = a_var;
        at.data.vr = $1;
        $$ = at;
    }
    | Val {
        atom at;
        at.type = a_val;
        at.data.vl = $1;
        $$ = at;
    }
    ;

Atom_left_equ : Atom G_EQUAL Equ_chain {
    if (glob_ps == NULL) glob_ps = new_vector(1, sizeof(expr));
    glob_ps =  $3;
    expr at = wrap_atom($1);
    vec_insert_at(glob_ps, 0, &at);
    expr res;
    res.type = e_equ_chain;
    res.e.ec.equs = glob_ps;
    
    $$ = res;
    glob_ps = NULL;
    
    /*
    expr lhs = wrap_atom($1);
    expr rhs = $3;
    equality eq;
    eq.lhs = malloc(sizeof(expr));
    eq.rhs = malloc(sizeof(expr));
    memcpy(eq.lhs, &lhs, sizeof(lhs));
    memcpy(eq.rhs, &rhs, sizeof(rhs));
    $$ = eq;*/
};

Ocall : Expr Operator Expr {
    fcall op = $2;
    op.params = new_vector(2, sizeof(expr));
    vec_push_back(op.params, &$1);
    vec_push_back(op.params, &$3);
    $$ = op;
};

Operator : G_PLUS         { $$ = fc_plus;  }
         | G_MINUS        { $$ = fc_minus; }
         | G_DIV          { $$ = fc_div;   }
         | G_MUL          { $$ = fc_mul;   }
         | G_LESS_THAN    { $$ = fc_lt;    }
         | G_GREATER_THAN { $$ = fc_gt;    }
         ;

Builtin_func : G_PRINT_F   { $$ = fc_print;   }
             | G_NL_F      { $$ = fc_nl;      }
             | G_INTEGER_F { $$ = fc_integer; }
             | G_CONS_F    { $$ = fc_cons;    }
             ;

Answer_count : G_FST_ANS { $$ = rs_first; }
             | G_ONE_ANS { $$ = rs_one;   }
             | G_ALL_ANS { $$ = rs_all;   }
             ;

%%

void globerror(char* s) {
    printf("Error, %s : On line %i. globerror()\n", s, glob_line_number);
    flag = 1;
}

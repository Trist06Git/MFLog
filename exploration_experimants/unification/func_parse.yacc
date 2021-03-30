%define api.prefix {func}
//%code provides {
//    #define YY_DECL int funclex (FUNCSTYPE *yylval, FUNCLTYPE *yylloc)
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

    int funcdebug = 1;
    extern int func_line_number;

    int unique = 0;
    bool unique_match = false;
    bool encountered_var = false;

    extern int flag;
    extern vector* func_defs;
    extern vector* global_defs;
    extern map* global_symbol_table;
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
    vector* ps = NULL;
    mf_array* ls = NULL;
    map* symbol_table = NULL;

    int funclex(void);
    void funcerror(char*);
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

%token <number> NUMBER
%token <string> WORD

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
//%type <u_ex> Atom_left_equ

%type <vec> Equ_chain
%type <u_ex> Expr_eq_ch

%left MINUS
%left PLUS
%left MUL
%left DIV
%token LP_ROUND RP_ROUND
%token END
%left EQUAL
%left LESS_THAN
%left GREATER_THAN
%left AND
%token PRINT_F
%token NL_F
%token INTEGER_F
%token CONS_F
%token REF_F
%token FST_ANS
%token ONE_ANS
%token ALL_ANS
%token LP_LIST RP_LIST
%left CONS_LIST
%token APP_LIST
%token AND_LIST
%token WILD_VAR

%%

Program : Func_def
        | Func_def       Program
        | Fact           Program
        | error Program { yyerrok; }
        ;

//Global : Atom_left_equ END {
//    //vec_push_back(global_defs, &$1);
//    //skip
//};

Func_def : Func_head Exprs END {
    //reset symbol_table for local scope
    if (symbol_table != NULL) {
        for (int i = 0; i < map_size(symbol_table); i++) {
            pair* kv = map_at_index(symbol_table, i);
            char** str = kv->fst;
            free(*str);
        }
        free_map(symbol_table);
        symbol_table = NULL;
    }
    
    function f = $1;
    f.type = fd_func;
    f.e.type = e_and;
    f.e.e.n.ands = $2;
    vec_push_back(func_defs, &f);
};

Func_head
    : WORD LP_ROUND Atom_params RP_ROUND EQUAL {
        function f;
        f.name = $1;
        f.params = $3;
        $$ = f;
    }
    | WORD LP_ROUND RP_ROUND EQUAL {/*no params*/
        function f;
        f.name = $1;
        f.params = new_vector(0, sizeof(expr));
        $$ = f;
    }
    ;

Fact : WORD LP_ROUND Atom_params RP_ROUND END {
    function f;
    f.type = fd_fact;
    f.name = $1;
    f.params = $3;
    vec_push_back(func_defs, &f);
}

Atom_params
    : Atom {
        ps = new_vector(1, sizeof(atom));
        vec_insert_at(ps, 0, &$1);
        $$ = ps;
        ps = NULL;
    }
    | Atom AND Atom_params {
        if (ps == NULL) ps = new_vector(1, sizeof(atom));
        ps =  $3;
        vec_insert_at(ps, 0, &$1);
        $$ = ps;
        ps = NULL;
    }
    ;

Expr_params
    : Expr {
        ps = new_vector(1, sizeof(expr));
        vec_insert_at(ps, 0, &$1);
        $$ = ps;
        ps = NULL;
    }
    | Expr AND Expr_params {
        if (ps == NULL) ps = new_vector(1, sizeof(expr));
        ps =  $3;
        vec_insert_at(ps, 0, &$1);
        $$ = ps;
        ps = NULL;
    }
    ;

Equ_chain
    : Expr {
        ps = new_vector(1, sizeof(expr));
        vec_insert_at(ps, 0, &$1);
        $$ = ps;
        ps = NULL;
    }
    | Expr EQUAL Equ_chain {
        if (ps == NULL) ps = new_vector(1, sizeof(expr));
        ps =  $3;
        vec_insert_at(ps, 0, &$1);
        $$ = ps;
        ps = NULL;
    }
    ;

Exprs
    : Expr_eq_ch {
        ps = new_vector(1, sizeof(expr));
        vec_insert_at(ps, 0, &$1);
        $$ = ps;
        ps = NULL;
    }
    | Expr_eq_ch AND Exprs {
        if (ps == NULL) ps = new_vector(1, sizeof(expr));
        ps = $3;
        vec_insert_at(ps, 0, &$1);
        $$ = ps;
        ps = NULL;
    }
    ;

Expr : Fcall_ans     { expr ex; ex.type = e_fcall; ex.e.f = $1; $$ = ex; }
     | Fbuiltin_ans  { expr ex; ex.type = e_fcall; ex.e.f = $1; $$ = ex; }
     | Fcall         { expr ex; ex.type = e_fcall; ex.e.f = $1; $$ = ex; }
     | Fbuiltin      { expr ex; ex.type = e_fcall; ex.e.f = $1; $$ = ex; }
     | Atom          { expr ex; ex.type = e_atom;  ex.e.a = $1; $$ = ex; }
     | Ocall         { expr ex; ex.type = e_fcall; ex.e.f = $1; $$ = ex; }
     | Cons_fcall    {
         //invent name for return var.
         if (symbol_table == NULL) {
            symbol_table = new_map(sizeof(char*), sizeof(int));
            set_fst_comparator(symbol_table, string_compare);
            set_snd_comparator(symbol_table, byte_compare);
         }
         int num = map_size(symbol_table);
         char* name = malloc(sizeof(char)+digits(num)+7);
         sprintf(name, "INTERN_%i", num);
         map_add(symbol_table, &name, &num);
         var v;
         v.symbol.type = s_var;
         v.symbol.num = num;
         v.symbol.scope = -1;
         expr ret_param = {.type = e_atom, .e.a = {.type = a_var, .data.vr = v}};
         fcall cons = $1;
         vec_push_back(cons.params, &ret_param);
         expr ex = {.type = e_fcall, .e.f = cons};
         $$ = ex;
     }
     ;

Expr_list
    : Expr {
        encountered_var = false;
        ls = new_mfarray(sizeof(expr));
        mfa_push_front(ls, &$1);
        encountered_var |= is_var_e(&$1);
        $$ = ls;
        ls = NULL;
    }
    | Expr AND_LIST Expr_list {
        ls = $3;
        mfa_push_front(ls, &$1);
        encountered_var |= is_var_e(&$1);
        $$ = ls;
        ls = NULL;
    }
    ;

List
    : LP_LIST Expr_list RP_LIST {
        //what if list has only vars??
        list l;
        l.has_vars = encountered_var;
        l.type = v_int;
        l.lst = $2;
        $$ = l;
    }
    | LP_LIST RP_LIST {
        list l;
        l.has_vars = false;
        l.lst = NULL;//empty list
        l.type = v_notype;
        $$ = l;
    }
    ;

Expr_eq_ch : Equ_chain {
    expr ex;
    if (vec_size($1) == 1) {
        expr* ex1 = vec_at($1, 0);
        ex = *ex1;
        free_vector($1);
    } else if (vec_size($1) == 2) {
        expr* ex1 = vec_at($1, 0);
        expr* ex2 = vec_at($1, 1);
        ex.type = e_equ;
        ex.e.e.lhs = malloc(sizeof(expr));
       *ex.e.e.lhs = *ex1;
        ex.e.e.rhs = malloc(sizeof(expr));
       *ex.e.e.rhs = *ex2;
        free_vector($1);
    } else {
        ex.type = e_equ_chain;
        ex.e.ec.equs = $1;
    }
    $$ = ex;
};

Fbuiltin : Builtin_func LP_ROUND Expr_params RP_ROUND {
    fcall fc = $1;
    fc.res_set = rs_one;
    fc.params = $3;
    $$ = fc;
};

Fbuiltin_ans : Answer_count Fbuiltin {
    fcall fc = $2;
    fc.res_set = $1;
    $$ = fc;
};

Fcall : WORD LP_ROUND Expr_params RP_ROUND {
    fcall fc;
    fc.res_set = rs_one;
    fc.name = $1;
    if (strcmp(fc.name, "plus") == 0 || strcmp(fc.name, "cons") == 0) {//truely disgusting, fix this..
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
};

Cons_fcall
    : Expr CONS_LIST Expr {
        fcall cons;
        cons.name = strdup("cons");
        cons.type = f_builtin;
        cons.res_set = rs_one;
        cons.params = new_vector(3, sizeof(expr));
        vec_push_back(cons.params, &$1);
        vec_push_back(cons.params, &$3);
        $$ = cons;
    }
    | Cons_fcall CONS_LIST Expr {
        fcall cons = $1;
        vec_push_back(cons.params, &$3);
        $$ = cons;
    }
    ;

Val
    : NUMBER {
        val v;
        v.type = v_int;
        v.v.i = $1;
        $$ = v;
    }
    | MINUS NUMBER {
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

Var
    : WORD {
        if (symbol_table == NULL) {
            symbol_table = new_map(sizeof(char*), sizeof(int));
            set_fst_comparator(symbol_table, string_compare);
            set_snd_comparator(symbol_table, byte_compare);
        }
        char* debug_test = $1;

        var v;
        v.symbol.type = s_var;
        v.symbol.scope = -1;

        int* found_num = snd(symbol_table, &debug_test);
        if (found_num == NULL) {
            found_num = snd(global_symbol_table, &debug_test);
            if (found_num == NULL) {
                //first occurance
                char* name = strdup($1);
                int num = map_size(symbol_table);
                map_add(symbol_table, &name, &num);
                v.symbol.num = num;
                v.symbol.scope = -1;
            } else {
                //found in globals
                v.symbol.scope = 0;
                v.symbol.num = *found_num;
            }
        } else {
            v.symbol.num = *found_num;
        }
        $$ = v;
    }
    | WILD_VAR {
        var v;
        v.symbol.type = s_wild;
        $$ = v;
    }
    ;

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

//Atom_left_equ : Atom EQUAL Equ_chain {
//    if (ps == NULL) ps = new_vector(1, sizeof(expr));
//    ps =  $3;
//    expr at = wrap_atom($1);
//    vec_insert_at(ps, 0, &at);
//    expr res;
//    res.type = e_equ_chain;
//    res.e.ec.equs = ps;
//    
//    $$ = res;
//    ps = NULL;
//    
//    /*
//    expr lhs = wrap_atom($1);
//    expr rhs = $3;
//    equality eq;
//    eq.lhs = malloc(sizeof(expr));
//    eq.rhs = malloc(sizeof(expr));
//    memcpy(eq.lhs, &lhs, sizeof(lhs));
//    memcpy(eq.rhs, &rhs, sizeof(rhs));
//    $$ = eq;*/
//};

Ocall : Expr Operator Expr {
    fcall op = $2;
    op.params = new_vector(2, sizeof(expr));
    vec_push_back(op.params, &$1);
    vec_push_back(op.params, &$3);
    $$ = op;
};

Operator : PLUS         { $$ = fc_plus;  }
         | MINUS        { $$ = fc_minus; }
         | DIV          { $$ = fc_div;   }
         | MUL          { $$ = fc_mul;   }
         | LESS_THAN    { $$ = fc_lt;    }
         | GREATER_THAN { $$ = fc_gt;    }
         ;

Builtin_func : PRINT_F   { $$ = fc_print;   }
             | NL_F      { $$ = fc_nl;      }
             | INTEGER_F { $$ = fc_integer; }
             | CONS_F    { $$ = fc_cons;    }
             ;

Answer_count : FST_ANS { $$ = rs_first; }
             | ONE_ANS { $$ = rs_one;   }
             | ALL_ANS { $$ = rs_all;   }
             ;

%%

void funcerror(char* s) {
    printf("Error, %s : On line %i. funcerror()\n", s, func_line_number);
    flag = 1;
}

%{
    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>
    #include "internal_struct.h"
    #include "generic_vector.h"
    #include "mlog_array.h"
    #include "utils.h"

    #include "debug_stuff.h"

    int yydebug = 1;
    extern int line_number;

    int unique = 0;
    bool unique_match = false;
    bool encountered_var = false;

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
    vector* ps = NULL;
    mf_array* ls = NULL;

    int yylex(void);
    void yyerror(char*);
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
//%type <u_eq> Equation
%type <u_ex> Atom_left_equ

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
%token FST_ANS
%token ONE_ANS
%token ALL_ANS
%token LP_LIST RP_LIST
%token CONS_LIST
%token APP_LIST
%token AND_LIST

%%

Program : Func_def
        | Func_def Program
        | Fact     Program
        | Global   Program
        ;

Global : Atom_left_equ END {
    //expr ex = {.type = e_equ, .e.e = $1};
    vec_push_back(global_defs, &$1);
};

Func_def : Func_head Exprs END {
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

Expr : Fcall_ans     { expr ex; ex.type = e_fcall; ex.e.f  = $1; $$ = ex; }
     | Fbuiltin_ans  { expr ex; ex.type = e_fcall; ex.e.f  = $1; $$ = ex; }
     | Fcall         { expr ex; ex.type = e_fcall; ex.e.f  = $1; $$ = ex; }
     | Fbuiltin      { expr ex; ex.type = e_fcall; ex.e.f  = $1; $$ = ex; }
     | Atom          { expr ex; ex.type = e_atom;  ex.e.a  = $1; $$ = ex; }
     | Ocall         { expr ex; ex.type = e_fcall; ex.e.f  = $1; $$ = ex; }
     
     ;

Expr_list
    : Expr {
        encountered_var = false;
        ls = new_mfarray(sizeof(expr));
        mfa_push_back(ls, &$1);
        encountered_var |= is_var_e(&$1);
        $$ = ls;
        ls = NULL;
    }
    | Expr AND_LIST Expr_list {
        ls = $3;
        mfa_push_back(ls, &$1);
        encountered_var |= is_var_e(&$1);
        $$ = ls;
        ls = NULL;
    }
    ;

List
    : LP_LIST Expr_list RP_LIST {
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
}

Fcall : WORD LP_ROUND Expr_params RP_ROUND {
    fcall fc;
    fc.res_set = rs_one;
    fc.name = $1;
    fc.params = $3;
    $$ = fc;
};

Fcall_ans : Answer_count Fcall {
    fcall fc = $2;
    fc.res_set = $1;
    $$ = fc;
}

Val
    : NUMBER {
        val v;
        v.type = v_int;
        v.v.i = $1;
        $$ = v;
    }
    | List {
        val v;
        v.type = v_list;
        v.v.l = $1;
        $$ = v;
    }
    ;

Var : WORD {
    var v;
    v.symbol = malloc(sizeof(char)*strlen($1)+1);
    memcpy(v.symbol, $1, strlen($1)+1);
    v.symbol[strlen($1)] = 0;
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

Atom_left_equ : Atom EQUAL Equ_chain {
    if (ps == NULL) ps = new_vector(1, sizeof(expr));
    ps =  $3;
    expr at = wrap_atom($1);
    vec_insert_at(ps, 0, &at);
    expr res;
    res.type = e_equ_chain;
    res.e.ec.equs = ps;
    
    $$ = res;
    ps = NULL;
    
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
             ;

Answer_count : FST_ANS { $$ = rs_first; }
             | ONE_ANS { $$ = rs_one;   }
             | ALL_ANS { $$ = rs_all;   }
             ;

%%

void yyerror(char* s) {
    printf("Error, %s\nOn line %i.\n", s, line_number);
    flag = 1;
}

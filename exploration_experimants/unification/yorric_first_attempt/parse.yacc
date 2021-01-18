%{
    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>
    #include "internal_struct.h"
    #include "generic_vector.h"
    #include "utils.h"

    int yydebug = 1;

    int unique = 0;
    bool unique_match = false;

    extern int flag;
    extern vector* func_defs;
    extern fcall fc_div;
    extern fcall fc_mul;
    extern fcall fc_plus;
    extern fcall fc_minus;
    vector* ps = NULL;

    int yylex(void);
    void yyerror(char*);
%}

%start Program

%union {
    char* string;
    int number;
    vector* vec;
    atom u_at;
    val u_val;
    var u_var;
    fcall u_fc;
    and u_and;
    equality u_eq;
    expr u_ex;
    function u_fn;
}

%type <u_fn> Func_head
%type <vec> Params
%type <u_ex> Exprs
%type <u_ex> Expr
%type <u_fc> Fcall
%type <u_val> Val
%type <u_var> Var
%type <u_at> Atom
%type <u_eq> Equation
%type <u_ex> Plus 

%token <number> NUMBER
%token <string> WORD
%token LP_ROUND RP_ROUND
//%token AND
//%token EQUAL
%token END
//%token DIV MUL PLUS MINUS

%left AND
%left EQUAL
%left PLUS MINUS
%left DIV MUL

%%

Program : Func_def { printf("parsing Func_def\n"); }
        | Func_def Program
        ;

Func_def : Func_head Exprs END {
    function f = $1;
    f.e = $2;
    
    atom unnamed;
    unnamed.type = e_var;
    unnamed.data.vr.symbol = malloc(sizeof(char)*digits(unique)+1);
    sprintf(unnamed.data.vr.symbol, "_%i", unique);
    push_back(f.params, &unnamed);

    insert_at(func_defs, 0, &f);
};

Func_head : WORD LP_ROUND Params RP_ROUND EQUAL {
    function f;
    f.name = $1;
    f.params = $3;
    $$ = f;
};

Params
    : Atom {
        ps = new_vector(1, sizeof(atom));
        insert_at(ps, 0, &$1);
        $$ = ps;
        ps = NULL;
    }
    | Atom AND Params {
        if (ps == NULL) ps = new_vector(1, sizeof(atom));
        ps =  $3;
        insert_at(ps, 0, &$1);
        $$ = ps;
        ps = NULL;
    }
    ;

Exprs
    : Expr {
        $$ = $1;
    }
    | Expr AND Exprs {
        expr lhs = $1;
        expr rhs = $3;
        expr ex;
        ex.type = e_and;
        ex.e.n.lhs = malloc(sizeof(expr));
        ex.e.n.rhs = malloc(sizeof(expr));
        memcpy(ex.e.n.lhs, &lhs, sizeof(lhs));
        memcpy(ex.e.n.rhs, &rhs, sizeof(rhs));
        $$ = ex;
    }
    ;

Expr : Fcall    { expr ex; ex.type = e_fcall; ex.e.f  = $1; $$ = ex; }
     /*| Var      { expr ex; ex.type = e_var;   ex.e.vr = $1; $$ = ex; }*/
     | Atom     { expr ex; ex.type = e_atom;  ex.e.a  = $1; $$ = ex; }
     | Equation { expr ex; ex.type = e_equ;   ex.e.e  = $1; $$ = ex; }
     | Plus     { $$ = $1;/*expr ex; ex.type = e_and;   ex.e.n  = $1; $$ = ex;*/ }
     ;

Fcall : WORD LP_ROUND Params RP_ROUND {
    fcall fc;
    fc.name = $1;
    fc.params = $3;

    atom unnamed;
    unnamed.type = e_var;
    unnamed.data.vr.symbol = malloc(sizeof(char)*digits(unique)+1);
    sprintf(unnamed.data.vr.symbol, "_%i", unique++);//down 1 stack, no need to incr??
    push_back(fc.params, &unnamed);
    $$ = fc;
};

Val : NUMBER {
    val v;
    v.n = $1;
    $$ = v;
};

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

Equation : Exprs EQUAL Exprs {
    expr lhs = $1;
    expr rhs = $3;
    equality eq;
    eq.lhs = malloc(sizeof(expr));
    eq.rhs = malloc(sizeof(expr));
    memcpy(eq.lhs, &lhs, sizeof(lhs));
    memcpy(eq.rhs, &rhs, sizeof(rhs));
    $$ = eq;
};

Plus : Expr PLUS Expr {
    unique = 0;
    expr lhs = $1;
    expr rhs = $3;
    
    fc_plus.params = new_vector(0, sizeof(atom));
    atom a_res;
    a_res.type = a_var;

    vector* v = new_vector(3, sizeof(expr));
    
    if (lhs.type == e_atom) {
        push_back(fc_plus.params, &lhs.e.a);
    } else {
        a_res.data.vr.symbol = malloc(sizeof(char)*digits(unique)+2);
        sprintf(a_res.data.vr.symbol, "_%i", unique++);
        push_back(fc_plus.params, &a_res);
        push_back(v, &lhs);
    }

    if (rhs.type == e_atom) {
        push_back(fc_plus.params, &rhs.e.a);
    } else {
        a_res.data.vr.symbol = malloc(sizeof(char)*digits(unique)+2);
        sprintf(a_res.data.vr.symbol, "_%i", unique++);
        push_back(fc_plus.params, &a_res);
        push_back(v, &rhs);
    }

    //return variable
    a_res.data.vr.symbol = malloc(sizeof(char)*digits(unique)+2);
    sprintf(a_res.data.vr.symbol, "_%i", unique);//dont increment, last in list
    push_back(fc_plus.params, &a_res);

    expr expr_plus;
    expr_plus.type = e_fcall;
    expr_plus.e.f = fc_plus;
    if (size(v) <= 0) {
        free_vector(v);
        $$ = expr_plus;
    } else {
        push_back(v, &expr_plus);
        and ands = append_and_init(v);
        expr expr_ands = { e_and, .e.n = ands };

        free_vector(v);
        $$ = expr_ands;
    }
};

%%

void yyerror(char* s) {
    printf("ERROR: %s\n", s);
    flag = 1;
}

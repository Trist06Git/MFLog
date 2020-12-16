%{
    #include <stdio.h>
    #include <string.h>
    #include "internal_struct.h"
    #include "generic_vector.h"

    int yydebug = 1;

    extern int flag;
    extern vector* func_defs;
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

%token <number> NUMBER
%token <string> WORD
%token LP_ROUND RP_ROUND
%token AND
%token EQUAL
%token END
%token DIV MUL PLUS MINUS

%%

Program : Func_def { printf("parsing Func_def\n"); }
        | Func_def Program
        ;

Func_def : Func_head Exprs END {
    function f = $1;
    f.e = $2;
    insert_at(func_defs, 0, &f);
};

Func_head : WORD LP_ROUND Params RP_ROUND EQUAL {
    function f;
    f.name = $1;
    f.params = $3;
    $$ = f;
};

Params
    : WORD {
        ps = new_vector(1, sizeof(char*));
        char* str = $1;
        insert_at(ps, 0, &str);
        $$ = ps;
        ps = NULL;
    }
    | WORD AND Params {
        if (ps == NULL) ps = new_vector(1, sizeof(atom));
        ps =  $3;
        atom a;
        a.type = a_var;
        a.data.vr = $1;
        insert_at(ps, 0, &a);
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
     | Var      { expr ex; ex.type = e_var;   ex.e.vr = $1; $$ = ex; }
     | Atom     { expr ex; ex.type = e_atom;  ex.e.a  = $1; $$ = ex; }
     | Equation { expr ex; ex.type = e_equ;   ex.e.e  = $1; $$ = ex; }
     /*| Plus     { expr ex; ex.type = e_fcall; ex.e.f = $1; $$ = ex; }*/
     ;

Fcall : WORD LP_ROUND Params RP_ROUND {
    fcall fc;
    fc.name = malloc(sizeof(char)*strlen($1)+1);
    memcpy(fc.name, $1, strlen($1)+1);
    fc.name[strlen($1)] = 0;
    fc.params = $3;
    $$ = fc;
};

Val : NUMBER {
    val v;
    v.n = $1;
};

Var : WORD {
    var v;
    v.symbol = malloc(sizeof(char)*strlen($1)+1);
    memcpy(v.symbol, $1, strlen($1)+1);
    v.symbol[strlen($1)] = 0;
    $$ = v;
};

Atom
    : /*WORD {
        atom at;
        at.type = a_string;
        at.data.s = $1;
        $$ = at;
    }
    |*/ NUMBER {
        atom at;
        at.type = a_val;
        at.data.vl.n = $1;
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

/*Plus : Expr PLUS Expr {
    expr lhs = $1;
    expr rhs = $3;
};*/

%%

void yyerror(char* s) {
    printf("ERROR: %s\n", s);
    flag = 1;
}

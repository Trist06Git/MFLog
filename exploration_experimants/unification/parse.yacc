%{
    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>
    #include "internal_struct.h"
    #include "generic_vector.h"
    #include "utils.h"

    int yydebug = 1;
    extern int line_number;

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

%token <number> NUMBER
%token <string> WORD

%type <u_at> Atom
%type <u_val> Val
%type <u_var> Var
%type <u_fn> Func_head
%type <vec> Atom_params
%type <vec> Expr_params
%type <u_ex> Expr
%type <u_ex> Exprs
%type <u_ex> Expr_eq
%type <u_fc> Fcall
%type <u_fc> Ocall
%type <u_fc> Operator
%type <u_eq> Equation

%left MINUS
%left PLUS
%left MUL
%left DIV
%token LP_ROUND RP_ROUND
%token END
%left EQUAL
%left AND

%%

Program : Func_def
        | Func_def Program
        ;

Func_def : Func_head Exprs END {
    function f = $1;
    f.e = $2;
    //insert_at(func_defs, 0, &f);
    push_back(func_defs, &f);
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

Atom_params
    : Atom {
        ps = new_vector(1, sizeof(atom));
        insert_at(ps, 0, &$1);
        $$ = ps;
        ps = NULL;
    }
    | Atom AND Atom_params {
        if (ps == NULL) ps = new_vector(1, sizeof(atom));
        ps =  $3;
        insert_at(ps, 0, &$1);
        $$ = ps;
        ps = NULL;
    }
    ;

Expr_params
    : Expr {
        ps = new_vector(1, sizeof(expr));
        insert_at(ps, 0, &$1);
        $$ = ps;
        ps = NULL;
    }
    | Expr AND Expr_params {
        if (ps == NULL) ps = new_vector(1, sizeof(expr));
        ps =  $3;
        insert_at(ps, 0, &$1);
        $$ = ps;
        ps = NULL;
    }
    ;

Exprs
    : Expr_eq {
        $$ = $1;
    }
    | Exprs AND Expr_eq {
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
     | Atom     { expr ex; ex.type = e_atom;  ex.e.a  = $1; $$ = ex; }
     | Ocall    { expr ex; ex.type = e_fcall; ex.e.f  = $1; $$ = ex; }
     ;

Expr_eq : Expr     { $$ = $1; }
        | Equation { expr ex; ex.type = e_equ;   ex.e.e  = $1; $$ = ex; }

Fcall : WORD LP_ROUND Expr_params RP_ROUND {
    fcall fc;
    fc.name = $1;
    fc.params = $3;
    $$ = fc;
};

Val : NUMBER {
    val v;
    v.type = v_int;
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

Equation : Expr EQUAL Expr_eq {
    expr lhs = $1;
    expr rhs = $3;
    equality eq;
    eq.lhs = malloc(sizeof(expr));
    eq.rhs = malloc(sizeof(expr));
    memcpy(eq.lhs, &lhs, sizeof(lhs));
    memcpy(eq.rhs, &rhs, sizeof(rhs));
    $$ = eq;
};

Ocall : Expr Operator Expr {
    fcall op = $2;
    op.params = new_vector(2, sizeof(expr));
    push_back(op.params, &$1);
    push_back(op.params, &$3);
    $$ = op;
};

Operator : PLUS  { $$ = fc_plus;  }
         | MINUS { $$ = fc_minus; }
         | DIV   { $$ = fc_div;   }
         | MUL   { $$ = fc_mul;   }
         ;

%%

void yyerror(char* s) {
    printf("Error, %s\nOn line %i.\n", s, line_number);
    flag = 1;
}

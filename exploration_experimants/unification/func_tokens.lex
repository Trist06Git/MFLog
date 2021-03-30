%option prefix="func"

%{
    #include <stdio.h>
    #include "internal_struct.h"
    #include "func.tab.h"

    extern void funcerror(char*);
    int func_line_number = 1;
    extern int verbose;

    //Notes
    //I cant seem to work out comments in lex so theyre all going up here...
    //Nested pattern substitution doesnt seem to work... :-/
    //see greek letters for details.
%}

%x MULTI_COMMENT
%x SINGLE_COMMENT

greek_lower    [αβγδεζηθικλμνξοπρσςτυφχψω]
greek_upper    [ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ]
digits         [0-9]
english        [A-Za-z]

%%

"print"   return PRINT_F;
"nl"      return NL_F;
"first"   return FST_ANS;
"one"     return ONE_ANS;
"all"     return ALL_ANS;
"integer" return INTEGER_F;
"cons"    return CONS_F;
"ref"     return REF_F;

{digits}+ {
    funclval.number = atoi(functext);
    return NUMBER;
}

({english}|{greek_lower}|{greek_upper})(_|{digits}|{english}|{greek_lower}|{greek_upper})* {
    funclval.string = strdup(functext);
    return WORD;
}

[_] return WILD_VAR;

[(] return LP_ROUND;
[)] return RP_ROUND;
[[] return LP_LIST;
[]] return RP_LIST;

"::" return CONS_LIST;
"++" return APP_LIST;
[;] return AND_LIST;

[,] return AND;
[=] return EQUAL;
[<] return LESS_THAN;
[>] return GREATER_THAN;
[.] return END;

[/] return DIV;
[*] return MUL;
[+] return PLUS;
[-] return MINUS;

[\t] ;
" " ;

[\n] func_line_number++;

"/*"                { BEGIN(MULTI_COMMENT); }
<MULTI_COMMENT>"*/" { BEGIN(INITIAL); }
<MULTI_COMMENT>.    { }
<MULTI_COMMENT>\n   { func_line_number++; }

"//"                { BEGIN(SINGLE_COMMENT); }
<SINGLE_COMMENT>.   { }
<SINGLE_COMMENT>\n  { BEGIN(INITIAL); func_line_number++; }

<<EOF>> { 
    if (verbose > 0) printf("End Of File.\n");
    yyterminate();
};
. return functext[0];


%%

int funcwrap() {
    return 1;
}

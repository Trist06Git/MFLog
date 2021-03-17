%option prefix="glob"

%{
    #include <stdio.h>
    #include "internal_struct.h"
    #include "glob.tab.h"

    extern void globerror(char*);
    int glob_line_number = 1;
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

"print"   return G_PRINT_F;
"nl"      return G_NL_F;
"first"   return G_FST_ANS;
"one"     return G_ONE_ANS;
"all"     return G_ALL_ANS;
"integer" return G_INTEGER_F;

{digits}+ {
    globlval.number = atoi(globtext);
    return G_NUMBER;
}

({english}|{greek_lower}|{greek_upper})(_|{digits}|{english}|{greek_lower}|{greek_upper})* {
    globlval.string = strdup(globtext);
    return G_WORD;
}

[_] return G_WILD_VAR;

[(] return G_LP_ROUND;
[)] return G_RP_ROUND;
[[] return G_LP_LIST;
[]] return G_RP_LIST;

"::" return G_CONS_LIST;
"++" return G_APP_LIST;
[;] return G_AND_LIST;

[,] return G_AND;
[=] return G_EQUAL;
[<] return G_LESS_THAN;
[>] return G_GREATER_THAN;
[.] return G_END;

[/] return G_DIV;
[*] return G_MUL;
[+] return G_PLUS;
[-] return G_MINUS;

[\t] ;
" " ;

[\n] glob_line_number++;

"/*"                { BEGIN(MULTI_COMMENT); }
<MULTI_COMMENT>"*/" { BEGIN(INITIAL); }
<MULTI_COMMENT>.    { }
<MULTI_COMMENT>\n   { glob_line_number++; }

"//"                { BEGIN(SINGLE_COMMENT); }
<SINGLE_COMMENT>.   { }
<SINGLE_COMMENT>\n  { BEGIN(INITIAL); glob_line_number++; }

<<EOF>> { 
    if (verbose > 0) printf("End Of File.\n");
    yyterminate();
};
. return globtext[0];


%%

int globwrap() {
    return 1;
}

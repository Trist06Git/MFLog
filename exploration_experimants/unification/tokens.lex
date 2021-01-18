%{
    #include <stdio.h>
    #include "internal_struct.h"
    #include "y.tab.h"

    extern void yyerror(char*);
    int line_number = 1;

    //Notes
    //I cant seem to work out comments in lex so theyre all going up here...
    //nested pattern substitution doesnt seem to work... :-/
%}

%x MULTI_COMMENT
%x SINGLE_COMMENT

greek_lower    [αβγδεζηθικλμνξοπρσςτυφχψω]
greek_upper    [ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ]
digits         [0-9]
english        [A-Za-z_]

%%

{digits}+ {
    yylval.number = atoi(yytext);
    return NUMBER;
}

({english}|{greek_lower}|{greek_upper})({digits}|{english}|{greek_lower}|{greek_upper})* {
    yylval.string = strdup(yytext);
    return WORD;
}

[(] return LP_ROUND;
[)] return RP_ROUND;

[,] return AND;
[=] return EQUAL;
[.] return END;

[/] return DIV;
[*] return MUL;
[+] return PLUS;
[-] return MINUS;

[\t] ;
" " ;

[\n] line_number++;

"/*"                { BEGIN(MULTI_COMMENT); }
<MULTI_COMMENT>"*/" { BEGIN(INITIAL); }
<MULTI_COMMENT>.    { }
<MULTI_COMMENT>\n   { line_number++; }

"//"                { BEGIN(SINGLE_COMMENT); }
<SINGLE_COMMENT>.   { }
<SINGLE_COMMENT>\n  { BEGIN(INITIAL); line_number++; }

<<EOF>> { 
    printf("End Of File.\n");
    yyterminate();
};
. return yytext[0];

%%

int yywrap() {
    return 1;
}

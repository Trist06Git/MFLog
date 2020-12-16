#!/bin/bash

yacc -d parse.yacc
lex tokens.lex
gcc main.c lex.yy.c y.tab.c

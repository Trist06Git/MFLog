/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_FUNC_FUNC_TAB_H_INCLUDED
# define YY_FUNC_FUNC_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef FUNCDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define FUNCDEBUG 1
#  else
#   define FUNCDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define FUNCDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined FUNCDEBUG */
#if FUNCDEBUG
extern int funcdebug;
#endif

/* Token type.  */
#ifndef FUNCTOKENTYPE
# define FUNCTOKENTYPE
  enum functokentype
  {
    NUMBER = 258,
    WORD = 259,
    MINUS = 260,
    PLUS = 261,
    MUL = 262,
    DIV = 263,
    LP_ROUND = 264,
    RP_ROUND = 265,
    END = 266,
    EQUAL = 267,
    LESS_THAN = 268,
    GREATER_THAN = 269,
    AND = 270,
    PRINT_F = 271,
    NL_F = 272,
    INTEGER_F = 273,
    FST_ANS = 274,
    ONE_ANS = 275,
    ALL_ANS = 276,
    LP_LIST = 277,
    RP_LIST = 278,
    CONS_LIST = 279,
    APP_LIST = 280,
    AND_LIST = 281,
    WILD_VAR = 282
  };
#endif
/* Tokens.  */
#define NUMBER 258
#define WORD 259
#define MINUS 260
#define PLUS 261
#define MUL 262
#define DIV 263
#define LP_ROUND 264
#define RP_ROUND 265
#define END 266
#define EQUAL 267
#define LESS_THAN 268
#define GREATER_THAN 269
#define AND 270
#define PRINT_F 271
#define NL_F 272
#define INTEGER_F 273
#define FST_ANS 274
#define ONE_ANS 275
#define ALL_ANS 276
#define LP_LIST 277
#define RP_LIST 278
#define CONS_LIST 279
#define APP_LIST 280
#define AND_LIST 281
#define WILD_VAR 282

/* Value type.  */
#if ! defined FUNCSTYPE && ! defined FUNCSTYPE_IS_DECLARED
union FUNCSTYPE
{
#line 49 "func_parse.yacc"

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

#line 136 "func.tab.h"

};
typedef union FUNCSTYPE FUNCSTYPE;
# define FUNCSTYPE_IS_TRIVIAL 1
# define FUNCSTYPE_IS_DECLARED 1
#endif


extern FUNCSTYPE funclval;

int funcparse (void);

#endif /* !YY_FUNC_FUNC_TAB_H_INCLUDED  */

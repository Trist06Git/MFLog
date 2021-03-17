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

#ifndef YY_GLOB_GLOB_TAB_H_INCLUDED
# define YY_GLOB_GLOB_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef GLOBDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define GLOBDEBUG 1
#  else
#   define GLOBDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define GLOBDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined GLOBDEBUG */
#if GLOBDEBUG
extern int globdebug;
#endif

/* Token type.  */
#ifndef GLOBTOKENTYPE
# define GLOBTOKENTYPE
  enum globtokentype
  {
    G_NUMBER = 258,
    G_WORD = 259,
    G_MINUS = 260,
    G_PLUS = 261,
    G_MUL = 262,
    G_DIV = 263,
    G_LP_ROUND = 264,
    G_RP_ROUND = 265,
    G_END = 266,
    G_EQUAL = 267,
    G_LESS_THAN = 268,
    G_GREATER_THAN = 269,
    G_AND = 270,
    G_PRINT_F = 271,
    G_NL_F = 272,
    G_INTEGER_F = 273,
    G_FST_ANS = 274,
    G_ONE_ANS = 275,
    G_ALL_ANS = 276,
    G_LP_LIST = 277,
    G_RP_LIST = 278,
    G_CONS_LIST = 279,
    G_APP_LIST = 280,
    G_AND_LIST = 281,
    G_WILD_VAR = 282
  };
#endif
/* Tokens.  */
#define G_NUMBER 258
#define G_WORD 259
#define G_MINUS 260
#define G_PLUS 261
#define G_MUL 262
#define G_DIV 263
#define G_LP_ROUND 264
#define G_RP_ROUND 265
#define G_END 266
#define G_EQUAL 267
#define G_LESS_THAN 268
#define G_GREATER_THAN 269
#define G_AND 270
#define G_PRINT_F 271
#define G_NL_F 272
#define G_INTEGER_F 273
#define G_FST_ANS 274
#define G_ONE_ANS 275
#define G_ALL_ANS 276
#define G_LP_LIST 277
#define G_RP_LIST 278
#define G_CONS_LIST 279
#define G_APP_LIST 280
#define G_AND_LIST 281
#define G_WILD_VAR 282

/* Value type.  */
#if ! defined GLOBSTYPE && ! defined GLOBSTYPE_IS_DECLARED
union GLOBSTYPE
{
#line 46 "glob_parse.yacc"

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

#line 136 "glob.tab.h"

};
typedef union GLOBSTYPE GLOBSTYPE;
# define GLOBSTYPE_IS_TRIVIAL 1
# define GLOBSTYPE_IS_DECLARED 1
#endif


extern GLOBSTYPE globlval;

int globparse (void);

#endif /* !YY_GLOB_GLOB_TAB_H_INCLUDED  */

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
    STRING_L = 260,
    MINUS = 261,
    PLUS = 262,
    MUL = 263,
    DIV = 264,
    LP_ROUND = 265,
    RP_ROUND = 266,
    END = 267,
    SET_LEFT = 268,
    SET_RIGHT = 269,
    SWAP = 270,
    EQUAL = 271,
    LESS_THAN = 272,
    GREATER_THAN = 273,
    AND = 274,
    PRINT_F = 275,
    NL_F = 276,
    INTEGER_F = 277,
    CONS_F = 278,
    REF_F = 279,
    CARD_F = 280,
    FST_ANS = 281,
    ONE_ANS = 282,
    ALL_ANS = 283,
    LP_LIST = 284,
    RP_LIST = 285,
    CONS_LIST = 286,
    APP_LIST = 287,
    AND_LIST = 288,
    WILD_VAR = 289,
    COLON = 290
  };
#endif
/* Tokens.  */
#define NUMBER 258
#define WORD 259
#define STRING_L 260
#define MINUS 261
#define PLUS 262
#define MUL 263
#define DIV 264
#define LP_ROUND 265
#define RP_ROUND 266
#define END 267
#define SET_LEFT 268
#define SET_RIGHT 269
#define SWAP 270
#define EQUAL 271
#define LESS_THAN 272
#define GREATER_THAN 273
#define AND 274
#define PRINT_F 275
#define NL_F 276
#define INTEGER_F 277
#define CONS_F 278
#define REF_F 279
#define CARD_F 280
#define FST_ANS 281
#define ONE_ANS 282
#define ALL_ANS 283
#define LP_LIST 284
#define RP_LIST 285
#define CONS_LIST 286
#define APP_LIST 287
#define AND_LIST 288
#define WILD_VAR 289
#define COLON 290

/* Value type.  */
#if ! defined FUNCSTYPE && ! defined FUNCSTYPE_IS_DECLARED
union FUNCSTYPE
{
#line 53 "func_parse.yacc"

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

#line 152 "func.tab.h"

};
typedef union FUNCSTYPE FUNCSTYPE;
# define FUNCSTYPE_IS_TRIVIAL 1
# define FUNCSTYPE_IS_DECLARED 1
#endif


extern FUNCSTYPE funclval;

int funcparse (void);

#endif /* !YY_FUNC_FUNC_TAB_H_INCLUDED  */

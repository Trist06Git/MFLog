/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Substitute the type names.  */
#define YYSTYPE         GLOBSTYPE
/* Substitute the variable and function names.  */
#define yyparse         globparse
#define yylex           globlex
#define yyerror         globerror
#define yydebug         globdebug
#define yynerrs         globnerrs
#define yylval          globlval
#define yychar          globchar

/* First part of user prologue.  */
#line 7 "glob_parse.yacc"

    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>
    #include "internal_struct.h"
    #include "generic_vector.h"
    #include "generic_map.h"
    #include "mlog_array.h"
    #include "utils.h"

    #include "debug_stuff.h"

    int globdebug = 1;
    extern int glob_line_number;

    bool glob_encountered_var = false;

    extern int flag;
    extern vector* func_defs;
    extern vector* global_defs;
    extern fcall fc_div;
    extern fcall fc_mul;
    extern fcall fc_plus;
    extern fcall fc_minus;
    extern fcall fc_print;
    extern fcall fc_nl;
    extern fcall fc_integer;
    extern fcall fc_lt;
    extern fcall fc_gt;
    extern map* global_symbol_table;
    vector* glob_ps = NULL;
    mf_array* glob_ls = NULL;

    int globlex(void);
    void globerror(char*);

#line 115 "glob.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
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

#line 246 "glob.tab.c"

};
typedef union GLOBSTYPE GLOBSTYPE;
# define GLOBSTYPE_IS_TRIVIAL 1
# define GLOBSTYPE_IS_DECLARED 1
#endif


extern GLOBSTYPE globlval;

int globparse (void);

#endif /* !YY_GLOB_GLOB_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined GLOBSTYPE_IS_TRIVIAL && GLOBSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  35
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   224

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  28
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  33
/* YYNRULES -- Number of rules.  */
#define YYNRULES  71
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  113

#define YYUNDEFTOK  2
#define YYMAXUTOK   282


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27
};

#if GLOBDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   115,   115,   116,   117,   118,   121,   126,   131,   134,
     139,   144,   147,   153,   156,   162,   166,   169,   174,   179,
     182,   188,   191,   197,   200,   206,   207,   208,   209,   210,
     211,   214,   217,   223,   229,   239,   242,   247,   248,   249,
     250,   251,   252,   257,   265,   275,   282,   290,   294,   301,
     307,   320,   327,   333,   339,   347,   368,   374,   382,   405,
     413,   414,   415,   416,   417,   418,   421,   422,   423,   426,
     427,   428
};
#endif

#if GLOBDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "G_NUMBER", "G_WORD", "G_MINUS",
  "G_PLUS", "G_MUL", "G_DIV", "G_LP_ROUND", "G_RP_ROUND", "G_END",
  "G_EQUAL", "G_LESS_THAN", "G_GREATER_THAN", "G_AND", "G_PRINT_F",
  "G_NL_F", "G_INTEGER_F", "G_FST_ANS", "G_ONE_ANS", "G_ALL_ANS",
  "G_LP_LIST", "G_RP_LIST", "G_CONS_LIST", "G_APP_LIST", "G_AND_LIST",
  "G_WILD_VAR", "$accept", "Program", "Global", "Func_def", "Func_head",
  "Fact", "Expr_list_skip", "List_skip", "Val_skip", "Var_skip",
  "Atom_skip", "Atom_params", "Expr_params", "Expr_skip", "Equ_chain_skip",
  "Equ_chain", "Exprs", "Expr", "Expr_list", "List", "Expr_eq_ch",
  "Fbuiltin", "Fbuiltin_ans", "Fcall", "Fcall_ans", "Val", "Var", "Atom",
  "Atom_left_equ", "Ocall", "Operator", "Builtin_func", "Answer_count", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282
};
# endif

#define YYPACT_NINF (-74)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-56)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      45,   -74,    13,     8,    24,     9,    45,    45,   136,    45,
     -74,   -74,   -74,    -5,    27,    80,   -74,    30,   -74,   -74,
     -74,   -74,   -74,   -74,   -74,     7,    28,   -74,   -74,   -74,
     -74,   -74,   -74,    47,    48,   -74,   -74,   -74,    11,   108,
      51,   107,   -74,   -74,   -74,   -74,    46,   -74,    70,   161,
      76,   165,   175,   179,   189,   193,   -74,   143,   -74,   -74,
     -74,    59,    82,   115,    81,    94,   136,   -74,   -74,   -74,
     -74,   -74,   -74,   143,   143,   -74,   136,    30,   -74,   -74,
     203,   207,    85,   150,   136,   -74,   136,   -74,   137,   -74,
     -74,   -74,    84,    19,   167,   176,   -74,   161,   180,   -74,
     136,   -74,   -74,   143,   -74,   -74,   -74,   -74,   136,   -74,
     -74,   -74,   -74
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    52,    55,     0,     0,     0,     0,     2,     0,     0,
      54,    57,    56,     0,     0,     0,    53,    55,    66,    67,
      68,    69,    70,    71,    46,    43,     0,    40,    38,    39,
      37,    41,    42,     0,     0,     1,     5,     3,    17,    18,
       0,     0,    15,    20,    19,    29,    31,    47,     0,     0,
      35,    28,    26,    27,    25,    30,     4,     0,     6,    17,
      18,     0,     0,     0,    21,     0,     0,    61,    60,    63,
      62,    64,    65,     0,     0,    45,     0,     0,    49,    51,
      16,    14,     0,    11,     0,     7,     0,    58,    33,    16,
       9,    14,     0,     0,     0,    23,    44,    59,     0,    13,
       0,    32,    36,     0,    22,    10,     8,    50,     0,    48,
      12,    34,    24
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -74,    91,   -74,   -74,   -74,   -74,   104,   -74,   -74,   -74,
     -10,   113,   -73,   -40,   134,   116,   138,    -4,   149,   -74,
     -74,    -2,     2,     0,     4,   -74,   -74,    86,   -74,     6,
     -74,   -74,   -74
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     5,     6,     7,     8,     9,    82,    42,    43,    44,
      45,    65,    94,    46,    47,    87,    48,    49,    26,    10,
      50,    51,    52,    53,    54,    11,    12,    31,    14,    55,
      74,    33,    34
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      25,    83,    27,    98,    29,    64,    28,    57,    30,    35,
      32,    16,    67,    68,    69,    70,   -52,   -52,   -52,   -52,
      71,    72,    15,    83,   -52,   -52,    95,     1,    17,     3,
     105,   106,    78,    73,    79,   112,    95,    25,    58,    66,
      18,    19,    20,    21,    22,    23,     4,    24,     1,     2,
       3,    75,    77,    88,    80,    27,    76,    29,    84,    28,
      83,    30,    89,    32,    18,    19,    20,     4,    95,    25,
      97,    27,    27,    29,    29,    28,    28,    30,    30,    32,
      32,    85,    64,    59,    60,    61,    13,    59,    60,    61,
      62,    86,    13,    13,    90,    13,    92,    36,    37,    88,
      56,    27,    63,    29,    93,    28,    63,    30,    99,    32,
      38,    39,    40,   -55,   -55,   -55,   -55,    66,    38,    39,
      40,   -55,   -55,    18,    19,    20,    21,    22,    23,    41,
      81,    18,    19,    20,    21,    22,    23,    41,    91,    38,
      39,    40,    67,    68,    69,    70,     1,    17,     3,   103,
      71,    72,    18,    19,    20,    21,    22,    23,    41,    18,
      19,    20,    21,    22,    23,     4,    67,    68,    69,    70,
     -40,   -40,   -40,   -40,    71,    72,   100,   107,   -40,   -40,
     -38,   -38,   -38,   -38,   -39,   -39,   -39,   -39,   -38,   -38,
     109,   108,   -39,   -39,   -37,   -37,   -37,   -37,   -42,   -42,
     -42,   -42,   -37,   -37,   110,   104,   -42,   -42,   -53,   -53,
     -53,   -53,   -46,   -46,   -46,   -46,   -53,   -53,   101,   111,
     -46,   -46,    96,     0,   102
};

static const yytype_int8 yycheck[] =
{
       4,    41,     4,    76,     4,    15,     4,    12,     4,     0,
       4,     3,     5,     6,     7,     8,     5,     6,     7,     8,
      13,    14,     9,    63,    13,    14,    66,     3,     4,     5,
      11,    12,    34,    26,    34,   108,    76,    41,    11,     9,
      16,    17,    18,    19,    20,    21,    22,    23,     3,     4,
       5,    23,     4,    57,     3,    57,     9,    57,    12,    57,
     100,    57,     3,    57,    16,    17,    18,    22,   108,    73,
      74,    73,    74,    73,    74,    73,    74,    73,    74,    73,
      74,    11,    92,     3,     4,     5,     0,     3,     4,     5,
      10,    15,     6,     7,    12,     9,    15,     6,     7,   103,
       9,   103,    22,   103,    10,   103,    22,   103,    23,   103,
       3,     4,     5,     5,     6,     7,     8,     9,     3,     4,
       5,    13,    14,    16,    17,    18,    19,    20,    21,    22,
      23,    16,    17,    18,    19,    20,    21,    22,    23,     3,
       4,     5,     5,     6,     7,     8,     3,     4,     5,    12,
      13,    14,    16,    17,    18,    19,    20,    21,    22,    16,
      17,    18,    19,    20,    21,    22,     5,     6,     7,     8,
       5,     6,     7,     8,    13,    14,    26,    10,    13,    14,
       5,     6,     7,     8,     5,     6,     7,     8,    13,    14,
      10,    15,    13,    14,     5,     6,     7,     8,     5,     6,
       7,     8,    13,    14,   100,    92,    13,    14,     5,     6,
       7,     8,     5,     6,     7,     8,    13,    14,    84,   103,
      13,    14,    73,    -1,    86
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,    22,    29,    30,    31,    32,    33,
      47,    53,    54,    55,    56,     9,     3,     4,    16,    17,
      18,    19,    20,    21,    23,    45,    46,    49,    50,    51,
      52,    55,    57,    59,    60,     0,    29,    29,     3,     4,
       5,    22,    35,    36,    37,    38,    41,    42,    44,    45,
      48,    49,    50,    51,    52,    57,    29,    12,    11,     3,
       4,     5,    10,    22,    38,    39,     9,     5,     6,     7,
       8,    13,    14,    26,    58,    23,     9,     4,    49,    51,
       3,    23,    34,    41,    12,    11,    15,    43,    45,     3,
      12,    23,    15,    10,    40,    41,    46,    45,    40,    23,
      26,    42,    44,    12,    39,    11,    12,    10,    15,    10,
      34,    43,    40
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    28,    29,    29,    29,    29,    30,    31,    32,    32,
      33,    34,    34,    35,    35,    36,    36,    36,    37,    38,
      38,    39,    39,    40,    40,    41,    41,    41,    41,    41,
      41,    42,    42,    43,    43,    44,    44,    45,    45,    45,
      45,    45,    45,    46,    46,    47,    47,    48,    49,    50,
      51,    52,    53,    53,    53,    54,    55,    55,    56,    57,
      58,    58,    58,    58,    58,    58,    59,    59,    59,    60,
      60,    60
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     2,     2,     2,     3,     5,     4,
       5,     1,     3,     3,     2,     1,     2,     1,     1,     1,
       1,     1,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     2,     1,     4,     2,
       4,     2,     1,     2,     1,     1,     1,     1,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if GLOBDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !GLOBDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !GLOBDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 6:
#line 121 "glob_parse.yacc"
                             {
    //expr ex = {.type = e_equ, .e.e = $1};
    vec_push_back(global_defs, &(yyvsp[-1].u_ex));
}
#line 1529 "glob.tab.c"
    break;

  case 7:
#line 126 "glob_parse.yacc"
                                 {
    //skip
}
#line 1537 "glob.tab.c"
    break;

  case 8:
#line 131 "glob_parse.yacc"
                                                       {
        //skip
    }
#line 1545 "glob.tab.c"
    break;

  case 9:
#line 134 "glob_parse.yacc"
                                           {/*no params*/
        //skip
    }
#line 1553 "glob.tab.c"
    break;

  case 10:
#line 139 "glob_parse.yacc"
                                                      {
    //skip
}
#line 1561 "glob.tab.c"
    break;

  case 11:
#line 144 "glob_parse.yacc"
                {
        //skip
    }
#line 1569 "glob.tab.c"
    break;

  case 12:
#line 147 "glob_parse.yacc"
                                          {
        //skip
    }
#line 1577 "glob.tab.c"
    break;

  case 13:
#line 153 "glob_parse.yacc"
                                         {
        //skip
    }
#line 1585 "glob.tab.c"
    break;

  case 14:
#line 156 "glob_parse.yacc"
                          {
        //skip
    }
#line 1593 "glob.tab.c"
    break;

  case 15:
#line 162 "glob_parse.yacc"
                {
        ///////truely horrific...
        //skip
    }
#line 1602 "glob.tab.c"
    break;

  case 16:
#line 166 "glob_parse.yacc"
                       {
        //skip
    }
#line 1610 "glob.tab.c"
    break;

  case 17:
#line 169 "glob_parse.yacc"
               {
        //skip
    }
#line 1618 "glob.tab.c"
    break;

  case 18:
#line 174 "glob_parse.yacc"
                  {
    //skip
}
#line 1626 "glob.tab.c"
    break;

  case 19:
#line 179 "glob_parse.yacc"
               {
        //skip
    }
#line 1634 "glob.tab.c"
    break;

  case 20:
#line 182 "glob_parse.yacc"
               {
        //skip
    }
#line 1642 "glob.tab.c"
    break;

  case 21:
#line 188 "glob_parse.yacc"
                {
        //skip
    }
#line 1650 "glob.tab.c"
    break;

  case 22:
#line 191 "glob_parse.yacc"
                                  {
        //skip
    }
#line 1658 "glob.tab.c"
    break;

  case 23:
#line 197 "glob_parse.yacc"
                {
        //skip
    }
#line 1666 "glob.tab.c"
    break;

  case 24:
#line 200 "glob_parse.yacc"
                                  {
        //skip
    }
#line 1674 "glob.tab.c"
    break;

  case 25:
#line 206 "glob_parse.yacc"
                    { /*whatever...*/ }
#line 1680 "glob.tab.c"
    break;

  case 26:
#line 207 "glob_parse.yacc"
                    { /*whatever...*/ }
#line 1686 "glob.tab.c"
    break;

  case 27:
#line 208 "glob_parse.yacc"
                    { /*whatever...*/ }
#line 1692 "glob.tab.c"
    break;

  case 28:
#line 209 "glob_parse.yacc"
                    { /*whatever...*/ }
#line 1698 "glob.tab.c"
    break;

  case 29:
#line 210 "glob_parse.yacc"
                    { }
#line 1704 "glob.tab.c"
    break;

  case 30:
#line 211 "glob_parse.yacc"
                    { /*whatever...*/ }
#line 1710 "glob.tab.c"
    break;

  case 31:
#line 214 "glob_parse.yacc"
                {
        //skip
    }
#line 1718 "glob.tab.c"
    break;

  case 32:
#line 217 "glob_parse.yacc"
                                       {
        //skip
    }
#line 1726 "glob.tab.c"
    break;

  case 33:
#line 223 "glob_parse.yacc"
           {
        glob_ps = new_vector(1, sizeof(expr));
        vec_insert_at(glob_ps, 0, &(yyvsp[0].u_ex));
        (yyval.vec) = glob_ps;
        glob_ps = NULL;
    }
#line 1737 "glob.tab.c"
    break;

  case 34:
#line 229 "glob_parse.yacc"
                             {
        if (glob_ps == NULL) glob_ps = new_vector(1, sizeof(expr));
        glob_ps =  (yyvsp[0].vec);
        vec_insert_at(glob_ps, 0, &(yyvsp[-2].u_ex));
        (yyval.vec) = glob_ps;
        glob_ps = NULL;
    }
#line 1749 "glob.tab.c"
    break;

  case 35:
#line 239 "glob_parse.yacc"
                 {
        //skip
    }
#line 1757 "glob.tab.c"
    break;

  case 36:
#line 242 "glob_parse.yacc"
                             {
        //skip
    }
#line 1765 "glob.tab.c"
    break;

  case 37:
#line 247 "glob_parse.yacc"
                     { expr ex; ex.type = e_fcall; ex.e.f  = (yyvsp[0].u_fc); (yyval.u_ex) = ex; }
#line 1771 "glob.tab.c"
    break;

  case 38:
#line 248 "glob_parse.yacc"
                     { expr ex; ex.type = e_fcall; ex.e.f  = (yyvsp[0].u_fc); (yyval.u_ex) = ex; }
#line 1777 "glob.tab.c"
    break;

  case 39:
#line 249 "glob_parse.yacc"
                     { expr ex; ex.type = e_fcall; ex.e.f  = (yyvsp[0].u_fc); (yyval.u_ex) = ex; }
#line 1783 "glob.tab.c"
    break;

  case 40:
#line 250 "glob_parse.yacc"
                     { expr ex; ex.type = e_fcall; ex.e.f  = (yyvsp[0].u_fc); (yyval.u_ex) = ex; }
#line 1789 "glob.tab.c"
    break;

  case 41:
#line 251 "glob_parse.yacc"
                     { expr ex; ex.type = e_atom;  ex.e.a  = (yyvsp[0].u_at); (yyval.u_ex) = ex; }
#line 1795 "glob.tab.c"
    break;

  case 42:
#line 252 "glob_parse.yacc"
                     { expr ex; ex.type = e_fcall; ex.e.f  = (yyvsp[0].u_fc); (yyval.u_ex) = ex; }
#line 1801 "glob.tab.c"
    break;

  case 43:
#line 257 "glob_parse.yacc"
           {
        glob_encountered_var = false;
        glob_ls = new_mfarray(sizeof(expr));
        mfa_push_back(glob_ls, &(yyvsp[0].u_ex));
        glob_encountered_var |= is_var_e(&(yyvsp[0].u_ex));
        (yyval.arr) = glob_ls;
        glob_ls = NULL;
    }
#line 1814 "glob.tab.c"
    break;

  case 44:
#line 265 "glob_parse.yacc"
                                {
        glob_ls = (yyvsp[0].arr);
        mfa_push_back(glob_ls, &(yyvsp[-2].u_ex));
        glob_encountered_var |= is_var_e(&(yyvsp[-2].u_ex));
        (yyval.arr) = glob_ls;
        glob_ls = NULL;
    }
#line 1826 "glob.tab.c"
    break;

  case 45:
#line 275 "glob_parse.yacc"
                                    {
        list l;
        l.has_vars = glob_encountered_var;
        l.type = v_int;
        l.lst = (yyvsp[-1].arr);
        (yyval.u_lst) = l;
    }
#line 1838 "glob.tab.c"
    break;

  case 46:
#line 282 "glob_parse.yacc"
                          {
        list l;
        l.has_vars = false;
        l.lst = NULL;//empty list
        (yyval.u_lst) = l;
    }
#line 1849 "glob.tab.c"
    break;

  case 47:
#line 290 "glob_parse.yacc"
                            {
    //skip
}
#line 1857 "glob.tab.c"
    break;

  case 48:
#line 294 "glob_parse.yacc"
                                                          {
    fcall fc = (yyvsp[-3].u_fc);
    fc.res_set = rs_one;
    fc.params = (yyvsp[-1].vec);
    (yyval.u_fc) = fc;
}
#line 1868 "glob.tab.c"
    break;

  case 49:
#line 301 "glob_parse.yacc"
                                     {
    fcall fc = (yyvsp[0].u_fc);
    fc.res_set = (yyvsp[-1].u_rs);
    (yyval.u_fc) = fc;
}
#line 1878 "glob.tab.c"
    break;

  case 50:
#line 307 "glob_parse.yacc"
                                                 {
    fcall fc;
    fc.res_set = rs_one;
    fc.name = (yyvsp[-3].string);
    if (strcmp(fc.name, "plus") == 0) {
        fc.type = f_builtin;
    } else {
        fc.type = f_user;
    }
    fc.params = (yyvsp[-1].vec);
    (yyval.u_fc) = fc;
}
#line 1895 "glob.tab.c"
    break;

  case 51:
#line 320 "glob_parse.yacc"
                               {
    fcall fc = (yyvsp[0].u_fc);
    fc.res_set = (yyvsp[-1].u_rs);
    (yyval.u_fc) = fc;
}
#line 1905 "glob.tab.c"
    break;

  case 52:
#line 327 "glob_parse.yacc"
               {
        val v;
        v.type = v_int;
        v.v.i = (yyvsp[0].number);
        (yyval.u_val) = v;
    }
#line 1916 "glob.tab.c"
    break;

  case 53:
#line 333 "glob_parse.yacc"
                       {
        val v;
        v.type = v_int;
        v.v.i = -(yyvsp[0].number);
        (yyval.u_val) = v;
    }
#line 1927 "glob.tab.c"
    break;

  case 54:
#line 339 "glob_parse.yacc"
           {
        val v;
        v.type = v_list;
        v.v.l = (yyvsp[0].u_lst);
        (yyval.u_val) = v;
    }
#line 1938 "glob.tab.c"
    break;

  case 55:
#line 347 "glob_parse.yacc"
             {
    var v;
    v.symbol.type = s_var;
    v.symbol.scope = 0;
    
    char* var_name = strdup((yyvsp[0].string));
    int* symbol_no = snd(global_symbol_table, &var_name);
    
    if (symbol_no == NULL) {
        //first encounter
        int i = map_size(global_symbol_table);
        map_add(global_symbol_table, &var_name, &i);
        v.symbol.num = i;
    } else {
        v.symbol.num = *symbol_no;
    }

    (yyval.u_var) = v;
}
#line 1962 "glob.tab.c"
    break;

  case 56:
#line 368 "glob_parse.yacc"
          {
        atom at;
        at.type = a_var;
        at.data.vr = (yyvsp[0].u_var);
        (yyval.u_at) = at;
    }
#line 1973 "glob.tab.c"
    break;

  case 57:
#line 374 "glob_parse.yacc"
          {
        atom at;
        at.type = a_val;
        at.data.vl = (yyvsp[0].u_val);
        (yyval.u_at) = at;
    }
#line 1984 "glob.tab.c"
    break;

  case 58:
#line 382 "glob_parse.yacc"
                                       {
    if (glob_ps == NULL) glob_ps = new_vector(1, sizeof(expr));
    glob_ps =  (yyvsp[0].vec);
    expr at = wrap_atom((yyvsp[-2].u_at));
    vec_insert_at(glob_ps, 0, &at);
    expr res;
    res.type = e_equ_chain;
    res.e.ec.equs = glob_ps;
    
    (yyval.u_ex) = res;
    glob_ps = NULL;
    
    /*
    expr lhs = wrap_atom($1);
    expr rhs = $3;
    equality eq;
    eq.lhs = malloc(sizeof(expr));
    eq.rhs = malloc(sizeof(expr));
    memcpy(eq.lhs, &lhs, sizeof(lhs));
    memcpy(eq.rhs, &rhs, sizeof(rhs));
    $$ = eq;*/
}
#line 2011 "glob.tab.c"
    break;

  case 59:
#line 405 "glob_parse.yacc"
                           {
    fcall op = (yyvsp[-1].u_fc);
    op.params = new_vector(2, sizeof(expr));
    vec_push_back(op.params, &(yyvsp[-2].u_ex));
    vec_push_back(op.params, &(yyvsp[0].u_ex));
    (yyval.u_fc) = op;
}
#line 2023 "glob.tab.c"
    break;

  case 60:
#line 413 "glob_parse.yacc"
                          { (yyval.u_fc) = fc_plus;  }
#line 2029 "glob.tab.c"
    break;

  case 61:
#line 414 "glob_parse.yacc"
                          { (yyval.u_fc) = fc_minus; }
#line 2035 "glob.tab.c"
    break;

  case 62:
#line 415 "glob_parse.yacc"
                          { (yyval.u_fc) = fc_div;   }
#line 2041 "glob.tab.c"
    break;

  case 63:
#line 416 "glob_parse.yacc"
                          { (yyval.u_fc) = fc_mul;   }
#line 2047 "glob.tab.c"
    break;

  case 64:
#line 417 "glob_parse.yacc"
                          { (yyval.u_fc) = fc_lt;    }
#line 2053 "glob.tab.c"
    break;

  case 65:
#line 418 "glob_parse.yacc"
                          { (yyval.u_fc) = fc_gt;    }
#line 2059 "glob.tab.c"
    break;

  case 66:
#line 421 "glob_parse.yacc"
                           { (yyval.u_fc) = fc_print;   }
#line 2065 "glob.tab.c"
    break;

  case 67:
#line 422 "glob_parse.yacc"
                           { (yyval.u_fc) = fc_nl;      }
#line 2071 "glob.tab.c"
    break;

  case 68:
#line 423 "glob_parse.yacc"
                           { (yyval.u_fc) = fc_integer; }
#line 2077 "glob.tab.c"
    break;

  case 69:
#line 426 "glob_parse.yacc"
                         { (yyval.u_rs) = rs_first; }
#line 2083 "glob.tab.c"
    break;

  case 70:
#line 427 "glob_parse.yacc"
                         { (yyval.u_rs) = rs_one;   }
#line 2089 "glob.tab.c"
    break;

  case 71:
#line 428 "glob_parse.yacc"
                         { (yyval.u_rs) = rs_all;   }
#line 2095 "glob.tab.c"
    break;


#line 2099 "glob.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 431 "glob_parse.yacc"


void globerror(char* s) {
    printf("Error, %s : On line %i. globerror()\n", s, glob_line_number);
    flag = 1;
}

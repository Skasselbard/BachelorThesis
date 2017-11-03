/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         ptformula_parse
#define yylex           ptformula_lex
#define yyerror         ptformula_error
#define yydebug         ptformula_debug
#define yynerrs         ptformula_nerrs

#define yylval          ptformula_lval
#define yychar          ptformula_char
#define yylloc          ptformula_lloc

/* Copy the first part of user declarations.  */
#line 28 "Frontend/Parser/ParserFormula.yy" /* yacc.c:339  */

#include <config.h>
#include <Core/Dimensions.h>
#include <CoverGraph/CoverGraph.h>
#include <Frontend/SymbolTable/ArcList.h>
#include <Frontend/SymbolTable/PlaceSymbol.h>
#include <Frontend/SymbolTable/TransitionSymbol.h>
#include <Frontend/SymbolTable/SymbolTable.h>
#include <Frontend/Parser/ParserPTNet.h>
#include <Frontend/Parser/error.h>
#include <Frontend/Parser/ast-system-k.h>
#include <Frontend/Parser/ast-system-yystype.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Marking.h>
#include <Formula/StatePredicate/DeadlockPredicate.h>
#include <Formula/StatePredicate/AtomicBooleanPredicate.h>
#include <Formula/StatePredicate/FireablePredicate.h>
#include <Formula/StatePredicate/TruePredicate.h>
#include <Formula/StatePredicate/FalsePredicate.h>
#include <Formula/StatePredicate/MagicNumber.h>

#include <limits.h>
#include <libgen.h>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <set>

#ifdef RERS
	extern bool * rers_place;
#endif

tShape oppShape(tShape);
extern ParserPTNet* symbolTables;
#line 133 "Frontend/Parser/ParserFormula.yy" /* yacc.c:339  */

// parser essentials
extern int ptformula_lex();
void ptformula_error(char const*);


std::set<arrayindex_t> target_place;
std::set<arrayindex_t> target_transition;
#line 143 "Frontend/Parser/ParserFormula.yy" /* yacc.c:339  */

/* globals */
tFormula TheFormula;

#line 125 "Frontend/Parser/ParserFormula.cc" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_PTFORMULA_FRONTEND_PARSER_PARSERFORMULA_HH_INCLUDED
# define YY_PTFORMULA_FRONTEND_PARSER_PARSERFORMULA_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ptformula_debug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    END = 0,
    _CTLOPERATOR_ = 258,
    IDENTIFIER = 259,
    NUMBER = 260,
    _RELEASE_ = 261,
    _NEXTSTATE_ = 262,
    _INITIAL_ = 263,
    _DEADLOCK_ = 264,
    _FORMULA_ = 265,
    _MAX_ = 266,
    _AND_ = 267,
    _NOT_ = 268,
    _OR_ = 269,
    _XOR_ = 270,
    _iff_ = 271,
    _ALLPATH_ = 272,
    _ALWAYS_ = 273,
    _EVENTUALLY_ = 274,
    _EXPATH_ = 275,
    _UNTIL_ = 276,
    _REACHABLE_ = 277,
    _INVARIANT_ = 278,
    _IMPOSSIBLE_ = 279,
    _notequal_ = 280,
    _implies_ = 281,
    _equals_ = 282,
    _plus_ = 283,
    _minus_ = 284,
    _times_ = 285,
    _leftparenthesis_ = 286,
    _rightparenthesis_ = 287,
    _greaterthan_ = 288,
    _lessthan_ = 289,
    _greaterorequal_ = 290,
    _lessorequal_ = 291,
    _semicolon_ = 292,
    _TRUE_ = 293,
    _FALSE_ = 294,
    _FIREABLE_ = 295,
    _omega_ = 296,
    _colon_ = 297
  };
#endif
/* Tokens.  */
#define END 0
#define _CTLOPERATOR_ 258
#define IDENTIFIER 259
#define NUMBER 260
#define _RELEASE_ 261
#define _NEXTSTATE_ 262
#define _INITIAL_ 263
#define _DEADLOCK_ 264
#define _FORMULA_ 265
#define _MAX_ 266
#define _AND_ 267
#define _NOT_ 268
#define _OR_ 269
#define _XOR_ 270
#define _iff_ 271
#define _ALLPATH_ 272
#define _ALWAYS_ 273
#define _EVENTUALLY_ 274
#define _EXPATH_ 275
#define _UNTIL_ 276
#define _REACHABLE_ 277
#define _INVARIANT_ 278
#define _IMPOSSIBLE_ 279
#define _notequal_ 280
#define _implies_ 281
#define _equals_ 282
#define _plus_ 283
#define _minus_ 284
#define _times_ 285
#define _leftparenthesis_ 286
#define _rightparenthesis_ 287
#define _greaterthan_ 288
#define _lessthan_ 289
#define _greaterorequal_ 290
#define _lessorequal_ 291
#define _semicolon_ 292
#define _TRUE_ 293
#define _FALSE_ 294
#define _FIREABLE_ 295
#define _omega_ 296
#define _colon_ 297

/* Value type.  */

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE ptformula_lval;
extern YYLTYPE ptformula_lloc;
int ptformula_parse (void);

#endif /* !YY_PTFORMULA_FRONTEND_PARSER_PARSERFORMULA_HH_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 271 "Frontend/Parser/ParserFormula.cc" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

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

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
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
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  45
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   208

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  43
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  8
/* YYNRULES -- Number of rules.  */
#define YYNRULES  49
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  95

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   297

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   151,   151,   152,   155,   157,   159,   161,   163,   168,
     170,   186,   217,   267,   318,   503,   559,   740,   742,   744,
     746,   748,   764,   766,   768,   779,   781,   783,   785,   792,
     805,   819,   833,   842,   855,   864,   871,   878,   891,   922,
     932,   939,   941,   957,   964,   969,   975,   980,   996,   997
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "$undefined", "\"CTL* operator\"",
  "\"identifier\"", "\"number\"", "\"temporal operator RELEASE\"",
  "\"temporal operator NEXTSTATE\"", "\"keyword INITIAL\"",
  "\"keyword DEADLOCK\"", "\"keyword FORMULA\"", "\"keyword MAX\"",
  "\"Boolean conjuction\"", "\"Boolean negation\"",
  "\"Boolean disjunction\"", "\"Boolean exclusive disjunction\"",
  "\"Boolean iff\"", "\"path quantifier ALLPATH\"",
  "\"temporal operator ALWAYS\"", "\"temporal operator EVENTUALLY\"",
  "\"path quantifier EXPATH\"", "\"temporal operator UNTIL\"",
  "\"keyword REACHABLE\"", "\"keyword INVARIANT\"",
  "\"keyword IMPOSSIBLE\"", "\"not-equals sign\"",
  "\"Boolean implication\"", "\"equals sign\"", "\"plus sign\"",
  "\"minus sign\"", "\"multiplication sign\"", "\"opening parenthesis\"",
  "\"closing parenthesis\"", "\"greater-than sign\"", "\"less-than sign\"",
  "\"greater-than-or-equal sign\"", "\"less-than-or-equal sign\"",
  "\"semicolon\"", "\"Boolean TRUE\"", "\"Boolean FALSE\"",
  "\"keyword FIREABLE\"", "\"omega\"", "\"colon\"", "$accept",
  "compoundformula", "formula", "statepredicate", "atomic_proposition",
  "computeboundformula", "term", "identifier", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297
};
# endif

#define YYPACT_NINF -24

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-24)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      65,   104,   -24,   -23,   104,   -24,   -24,   104,   -12,   104,
     104,   104,   104,   104,   104,   104,   104,   104,   -24,   -24,
      -3,   -24,     1,   -24,    26,   -24,   -24,   172,   -24,   -24,
      20,   -24,   142,    20,   -24,   -24,   -24,   -24,   -24,     6,
       6,     6,   134,    66,    23,   -24,    65,   104,   104,   104,
     104,   104,   -24,    20,    20,    20,    20,    20,    20,    20,
      20,   -24,    20,   -24,   -24,     7,   104,   104,   104,   -24,
     -24,     2,   -24,   -24,    17,    17,   170,   -10,    16,    16,
     -24,   -24,    16,    16,    16,    16,    86,   -24,   149,   157,
     162,   -24,   -24,   -24,   -24
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    49,    48,    43,     0,    38,    39,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    35,    36,
       0,    47,     0,     2,     6,    10,     8,     0,    42,    21,
       0,    25,     5,     0,    11,    17,    19,    20,    18,    26,
      27,    28,     0,     0,     0,     1,     0,     0,     0,     0,
       0,     0,     7,     0,     0,     0,     0,     0,     0,     0,
       0,    49,     0,    46,     4,     0,     0,     0,     0,     9,
      41,     0,     3,    12,    13,    14,    16,    15,    30,    29,
      44,    45,    31,    33,    32,    34,     0,    40,     0,     0,
       0,    37,    24,    23,    22
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -24,   -24,   -15,    -1,   -24,   -24,     0,    -7
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    22,    23,    24,    25,    26,    27,    28
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      29,    45,    47,    31,    48,    49,    32,    30,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    47,    33,
      48,    49,    50,    61,     2,     3,    61,     2,    44,    47,
      63,    72,    51,    65,    91,    55,    56,    71,    47,    87,
      48,    49,    50,    46,    55,    56,    73,    74,    75,    76,
      77,    62,    51,    78,    79,    80,    81,    82,    83,    84,
      85,    21,    86,    52,     0,    88,    89,    90,     1,     2,
       3,     0,     4,     5,     6,     7,     8,     0,     9,     0,
       0,     0,    10,    11,    12,    13,     0,    14,    15,    16,
       0,    53,     0,    54,    55,    56,    17,     0,    70,    57,
      58,    59,    60,    18,    19,    20,    21,     1,     2,     3,
       0,     4,     5,     6,    55,    56,     0,     9,    70,     0,
       0,    10,    11,    12,    13,     0,    14,    15,    16,     0,
       0,     0,     0,     0,     0,    17,     0,    66,     0,     0,
      67,     0,    18,    19,    20,    21,    47,     0,    48,    49,
      50,     0,     0,     0,    47,    68,    48,    49,    50,     0,
      51,    47,     0,    48,    49,    50,    69,     0,    51,    47,
       0,    48,    49,    50,    47,    51,    48,    49,    50,    64,
       0,    92,    47,    51,    48,    49,     0,     0,    51,    93,
       0,     0,     0,     0,    94,     0,    51,    53,     0,    54,
      55,    56,     0,     0,     0,    57,    58,    59,    60
};

static const yytype_int8 yycheck[] =
{
       1,     0,    12,     4,    14,    15,     7,    30,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    17,    12,    31,
      14,    15,    16,     3,     4,     5,     3,     4,    31,    12,
      30,    46,    26,    33,    32,    28,    29,    44,    12,    32,
      14,    15,    16,    42,    28,    29,    47,    48,    49,    50,
      51,    31,    26,    53,    54,    55,    56,    57,    58,    59,
      60,    41,    62,    37,    -1,    66,    67,    68,     3,     4,
       5,    -1,     7,     8,     9,    10,    11,    -1,    13,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    22,    23,    24,
      -1,    25,    -1,    27,    28,    29,    31,    -1,    32,    33,
      34,    35,    36,    38,    39,    40,    41,     3,     4,     5,
      -1,     7,     8,     9,    28,    29,    -1,    13,    32,    -1,
      -1,    17,    18,    19,    20,    -1,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,     3,    -1,    -1,
       6,    -1,    38,    39,    40,    41,    12,    -1,    14,    15,
      16,    -1,    -1,    -1,    12,    21,    14,    15,    16,    -1,
      26,    12,    -1,    14,    15,    16,    32,    -1,    26,    12,
      -1,    14,    15,    16,    12,    26,    14,    15,    16,    37,
      -1,    32,    12,    26,    14,    15,    -1,    -1,    26,    32,
      -1,    -1,    -1,    -1,    32,    -1,    26,    25,    -1,    27,
      28,    29,    -1,    -1,    -1,    33,    34,    35,    36
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     7,     8,     9,    10,    11,    13,
      17,    18,    19,    20,    22,    23,    24,    31,    38,    39,
      40,    41,    44,    45,    46,    47,    48,    49,    50,    46,
      30,    46,    46,    31,    46,    46,    46,    46,    46,    46,
      46,    46,    46,    49,    31,     0,    42,    12,    14,    15,
      16,    26,    37,    25,    27,    28,    29,    33,    34,    35,
      36,     3,    31,    49,    37,    49,     3,     6,    21,    32,
      32,    50,    45,    46,    46,    46,    46,    46,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    32,    46,    46,
      46,    32,    32,    32,    32
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    43,    44,    44,    45,    45,    45,    45,    45,    46,
      46,    46,    46,    46,    46,    46,    46,    46,    46,    46,
      46,    46,    46,    46,    46,    46,    46,    46,    46,    47,
      47,    47,    47,    47,    47,    47,    47,    47,    47,    47,
      48,    49,    49,    49,    49,    49,    49,    49,    50,    50
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     3,     2,     1,     2,     1,     3,
       1,     2,     3,     3,     3,     3,     3,     2,     2,     2,
       2,     2,     5,     5,     5,     2,     2,     2,     2,     3,
       3,     3,     3,     3,     3,     1,     1,     4,     1,     1,
       4,     3,     1,     1,     3,     3,     3,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
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


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


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
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
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
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
            /* Fall through.  */
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

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
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
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

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
      int yyn = yypact[*yyssp];
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
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
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
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
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
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
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
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
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
| yyreduce -- Do a reduction.  |
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

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 151 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tFormula) = TheFormula = (yyvsp[0].yt_tFormula);}
#line 1535 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 3:
#line 152 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    {(yyval.yt_tFormula) = TheFormula = Compound((yyvsp[-2].yt_tFormula),(yyvsp[0].yt_tFormula));}
#line 1541 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 4:
#line 156 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tFormula) = StatePredicateFormula((yyvsp[-1].yt_tStatePredicate)); }
#line 1547 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 5:
#line 158 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tFormula) = StatePredicateFormula((yyvsp[0].yt_tStatePredicate)); }
#line 1553 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 6:
#line 160 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tFormula) = StatePredicateFormula((yyvsp[0].yt_tStatePredicate)); }
#line 1559 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 7:
#line 162 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tFormula) = StatePredicateFormula((yyvsp[-1].yt_tStatePredicate)); }
#line 1565 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 8:
#line 164 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tFormula) = (yyvsp[0].yt_tFormula); }
#line 1571 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 9:
#line 169 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tStatePredicate) = (yyvsp[-1].yt_tStatePredicate); }
#line 1577 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 10:
#line 171 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	if((yyvsp[0].yt_tAtomicProposition)->pred->magicnumber == MAGIC_NUMBER_TRUE)
	{
		delete (yyvsp[0].yt_tAtomicProposition)->pred;
		(yyvsp[0].yt_tAtomicProposition) -> pred = new TruePredicate();
	}
	else if((yyvsp[0].yt_tAtomicProposition)->pred->magicnumber == MAGIC_NUMBER_FALSE)
	{
		delete (yyvsp[0].yt_tAtomicProposition)->pred;
		(yyvsp[0].yt_tAtomicProposition) -> pred = new FalsePredicate();
	}
	(yyval.yt_tStatePredicate) = AtomicProposition((yyvsp[0].yt_tAtomicProposition)); 
	(yyval.yt_tStatePredicate) -> shape = (yyvsp[0].yt_tAtomicProposition) -> shape;
	(yyval.yt_tStatePredicate) -> formula = (yyvsp[0].yt_tAtomicProposition) -> pred;
    }
#line 1597 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 11:
#line 187 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	switch((yyvsp[0].yt_tStatePredicate) -> shape)
	{
	case AT_TEMP: 	(yyval.yt_tStatePredicate) = Negation((yyvsp[0].yt_tStatePredicate)); 
			break;
	case AT_DL:
	case AT_FIR:
	case AT_COMP: 	(yyvsp[0].yt_tStatePredicate) -> formula = (yyvsp[0].yt_tStatePredicate) -> formula -> negate(); 
			(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate); 
			break;
	case AT_TRUE: 	delete((yyvsp[0].yt_tStatePredicate) -> formula);
			(yyvsp[0].yt_tStatePredicate) -> formula = new FalsePredicate();
			(yyvsp[0].yt_tStatePredicate) -> shape = AT_FALSE;
			(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
			break;
	case AT_FALSE: 	delete((yyvsp[0].yt_tStatePredicate) -> formula);
			(yyvsp[0].yt_tStatePredicate) -> formula = new TruePredicate();
			(yyvsp[0].yt_tStatePredicate) -> shape = AT_TRUE;
			(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
			break;
	case AT_AND: 	(yyvsp[0].yt_tStatePredicate) -> formula -> negate();
			(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
			(yyval.yt_tStatePredicate) -> shape = AT_OR;
			break;
	case AT_OR: 	(yyvsp[0].yt_tStatePredicate) -> formula -> negate();
			(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
			(yyval.yt_tStatePredicate) -> shape = AT_AND;
			break;
	}
    }
#line 1632 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 12:
#line 218 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_TEMP || (yyvsp[0].yt_tStatePredicate) -> shape == AT_TEMP)
	{
		(yyval.yt_tStatePredicate) = Conjunction((yyvsp[-2].yt_tStatePredicate), (yyvsp[0].yt_tStatePredicate)); 
		(yyval.yt_tStatePredicate) -> shape = AT_TEMP;
	}
	else if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_FALSE || (yyvsp[0].yt_tStatePredicate) -> shape == AT_FALSE)
	{
		delete (yyvsp[-2].yt_tStatePredicate) -> formula;
		delete (yyvsp[0].yt_tStatePredicate) -> formula;
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = new FalsePredicate();
		(yyval.yt_tStatePredicate) -> shape = AT_FALSE;
	}
	else if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_TRUE)
	{
		(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> shape = (yyvsp[0].yt_tStatePredicate) -> shape;
	}
	else if((yyvsp[0].yt_tStatePredicate) -> shape == AT_TRUE)
	{
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> shape = (yyvsp[-2].yt_tStatePredicate) -> shape;
	} 
	else if(((yyvsp[-2].yt_tStatePredicate)->shape == AT_AND) && (((yyvsp[0].yt_tStatePredicate) -> shape == AT_OR) || ((yyvsp[0].yt_tStatePredicate) -> shape == AT_FIR) || ((yyvsp[0].yt_tStatePredicate) -> shape == AT_COMP) || ((yyvsp[0].yt_tStatePredicate)->shape == AT_DL)))
	{
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[-2].yt_tStatePredicate) -> formula) -> addSub((yyvsp[0].yt_tStatePredicate)->formula);
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
	}
	else if(((yyvsp[0].yt_tStatePredicate)->shape == AT_AND) && (((yyvsp[-2].yt_tStatePredicate) -> shape == AT_OR) || ((yyvsp[-2].yt_tStatePredicate) -> shape == AT_COMP) || ((yyvsp[-2].yt_tStatePredicate) -> shape == AT_DL) || ((yyvsp[-2].yt_tStatePredicate) -> shape == AT_FIR)))
	{
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[0].yt_tStatePredicate) -> formula) -> addSub((yyvsp[-2].yt_tStatePredicate)->formula);
		(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
	}
	else if(((yyvsp[-2].yt_tStatePredicate)->shape == AT_AND) && ((yyvsp[0].yt_tStatePredicate) -> shape == AT_AND))
	{
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[0].yt_tStatePredicate) -> formula)->merge(reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[-2].yt_tStatePredicate)->formula));
		(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
	}
	else // both $1 and $3 are AT_FIR, AT_COMP, AT_DL or AT_OR
	{
		AtomicBooleanPredicate * result = new AtomicBooleanPredicate(true);
		result -> addSub((yyvsp[-2].yt_tStatePredicate)->formula);
		result -> addSub((yyvsp[0].yt_tStatePredicate)->formula);
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = result;
		(yyval.yt_tStatePredicate) -> shape = AT_AND;
	}
    }
#line 1686 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 13:
#line 268 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_TEMP || (yyvsp[0].yt_tStatePredicate) -> shape == AT_TEMP)
	{
		(yyval.yt_tStatePredicate) = Disjunction((yyvsp[-2].yt_tStatePredicate), (yyvsp[0].yt_tStatePredicate)); 
		(yyval.yt_tStatePredicate) -> shape = AT_TEMP;
	}
	else if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_TRUE || (yyvsp[0].yt_tStatePredicate) -> shape == AT_TRUE)
	{
		delete (yyvsp[-2].yt_tStatePredicate) -> formula;
		delete (yyvsp[0].yt_tStatePredicate) -> formula;
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = new TruePredicate();
		(yyval.yt_tStatePredicate) -> shape = AT_TRUE;
	}
	else if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_FALSE)
	{
		(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> shape = (yyvsp[0].yt_tStatePredicate) -> shape;
	}
	else if((yyvsp[0].yt_tStatePredicate) -> shape == AT_FALSE)
	{
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> shape = (yyvsp[-2].yt_tStatePredicate) -> shape;
	} 
	else if(((yyvsp[-2].yt_tStatePredicate)->shape == AT_OR) && (((yyvsp[0].yt_tStatePredicate) -> shape == AT_AND) || ((yyvsp[0].yt_tStatePredicate) -> shape == AT_COMP) || ((yyvsp[0].yt_tStatePredicate) -> shape == AT_FIR) || ((yyvsp[0].yt_tStatePredicate)->shape == AT_DL) ))
	{
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[-2].yt_tStatePredicate) -> formula) -> addSub((yyvsp[0].yt_tStatePredicate)->formula);
		
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
	}
	else if(((yyvsp[0].yt_tStatePredicate)->shape == AT_OR) && (((yyvsp[-2].yt_tStatePredicate) -> shape == AT_AND) || ((yyvsp[-2].yt_tStatePredicate) -> shape == AT_COMP) || ((yyvsp[-2].yt_tStatePredicate)->shape == AT_FIR) || ((yyvsp[-2].yt_tStatePredicate)->shape == AT_DL)))
	{
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[0].yt_tStatePredicate) -> formula) -> addSub((yyvsp[-2].yt_tStatePredicate)->formula);
		(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
	}
	else if(((yyvsp[-2].yt_tStatePredicate)->shape == AT_OR) && ((yyvsp[0].yt_tStatePredicate) -> shape == AT_OR))
	{
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[0].yt_tStatePredicate) -> formula)->merge(reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[-2].yt_tStatePredicate)->formula));
		(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
	}
	else // both $1 and $3 are AT_COMP,AT_FIR,AT_DL or AT_OR
	{
		AtomicBooleanPredicate * result = new AtomicBooleanPredicate(false);
		result -> addSub((yyvsp[-2].yt_tStatePredicate)->formula);
		result -> addSub((yyvsp[0].yt_tStatePredicate)->formula);
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = result;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
    }
#line 1741 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 14:
#line 319 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	// translate into (p & -q) | (-p & q)

	if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_TEMP || (yyvsp[0].yt_tStatePredicate) -> shape == AT_TEMP)
	{
		(yyval.yt_tStatePredicate) = ExclusiveDisjunction((yyvsp[-2].yt_tStatePredicate), (yyvsp[0].yt_tStatePredicate)); 
		(yyval.yt_tStatePredicate) -> shape = AT_TEMP;
	}
	else if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_TRUE)
	{
		(yyvsp[0].yt_tStatePredicate) -> formula = (yyvsp[0].yt_tStatePredicate) -> formula -> negate();
		(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> shape = oppShape((yyvsp[0].yt_tStatePredicate)->shape);
	}
	else if((yyvsp[0].yt_tStatePredicate) -> shape == AT_TRUE)
	{
		(yyvsp[-2].yt_tStatePredicate) -> formula = (yyvsp[-2].yt_tStatePredicate) -> formula -> negate();
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> shape = oppShape((yyvsp[-2].yt_tStatePredicate)->shape);
	}
	else if((yyvsp[0].yt_tStatePredicate) -> shape == AT_FALSE)
	{
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
	}
	else if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_FALSE)
	{
		(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
	}
	else if(((yyvsp[-2].yt_tStatePredicate) -> shape == AT_AND) && (((yyvsp[0].yt_tStatePredicate) -> shape == AT_COMP)|| ((yyvsp[0].yt_tStatePredicate)->shape == AT_FIR) || ((yyvsp[0].yt_tStatePredicate)->shape == AT_DL)))
	{
		StatePredicate * b = (yyvsp[0].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_*
		StatePredicate * c = (yyvsp[-2].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_OR
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(c);
		d -> addSub((yyvsp[0].yt_tStatePredicate)->formula); // d = (-p & q)
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[-2].yt_tStatePredicate) -> formula) ->addSub(b);     // $1 = (p & -q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub((yyvsp[-2].yt_tStatePredicate)->formula);
		a -> addSub(d); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if((((yyvsp[-2].yt_tStatePredicate) -> shape == AT_COMP)||((yyvsp[-2].yt_tStatePredicate)->shape == AT_FIR) || ((yyvsp[-2].yt_tStatePredicate)->shape == AT_DL)) && ((yyvsp[0].yt_tStatePredicate) -> shape == AT_AND))
	{
		StatePredicate * b = (yyvsp[-2].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_ELEM
		StatePredicate * c = (yyvsp[0].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_OR
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(c);
		d -> addSub((yyvsp[-2].yt_tStatePredicate)->formula); // d = (p & -q)
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[0].yt_tStatePredicate) -> formula) ->addSub(b);     // $3 = (-p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub((yyvsp[0].yt_tStatePredicate)->formula); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if((((yyvsp[-2].yt_tStatePredicate) -> shape == AT_COMP)||((yyvsp[-2].yt_tStatePredicate)->shape == AT_FIR) || ((yyvsp[-2].yt_tStatePredicate)->shape == AT_DL)) && ((yyvsp[0].yt_tStatePredicate) -> shape == AT_OR))
	{
		StatePredicate * b = (yyvsp[-2].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_ELEM
		StatePredicate * c = (yyvsp[0].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_AND
		reinterpret_cast<AtomicBooleanPredicate *>(c) -> addSub((yyvsp[-2].yt_tStatePredicate) -> formula); // c = p & -q
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(b);
		d -> addSub((yyvsp[0].yt_tStatePredicate)->formula); // d = (-p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub(c); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if(((yyvsp[-2].yt_tStatePredicate) -> shape == AT_OR) && (((yyvsp[0].yt_tStatePredicate) -> shape == AT_COMP)||((yyvsp[0].yt_tStatePredicate)->shape == AT_FIR) || ((yyvsp[0].yt_tStatePredicate)->shape == AT_DL)))
	{
		StatePredicate * b = (yyvsp[0].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_ELEM
		StatePredicate * c = (yyvsp[-2].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_AND
		reinterpret_cast<AtomicBooleanPredicate *>(c) -> addSub((yyvsp[0].yt_tStatePredicate) -> formula); // c = -p & q
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(b);
		d -> addSub((yyvsp[-2].yt_tStatePredicate)->formula); // d = (p & -q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub(c); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if((((yyvsp[-2].yt_tStatePredicate) -> shape == AT_COMP)||((yyvsp[-2].yt_tStatePredicate)->shape == AT_FIR) || ((yyvsp[-2].yt_tStatePredicate)->shape == AT_DL)) && (((yyvsp[0].yt_tStatePredicate) -> shape == AT_COMP)|| ((yyvsp[0].yt_tStatePredicate)->shape == AT_FIR)||((yyvsp[0].yt_tStatePredicate)->shape==AT_DL)))
	{
		StatePredicate * b = (yyvsp[0].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_ELEM
		StatePredicate * c = (yyvsp[-2].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_AND
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(b);
		d -> addSub((yyvsp[-2].yt_tStatePredicate)->formula); // d = (p & -q)
		AtomicBooleanPredicate * e = new AtomicBooleanPredicate(true);
		e -> addSub(c);
		e -> addSub((yyvsp[0].yt_tStatePredicate)->formula); // d = (p & -q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub(e); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if(((yyvsp[-2].yt_tStatePredicate) -> shape == AT_AND) && ((yyvsp[0].yt_tStatePredicate) -> shape == AT_AND))
	{
		StatePredicate * b = (yyvsp[0].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_OR
		StatePredicate * c = (yyvsp[-2].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_OR
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(b);
		d -> addSub((yyvsp[-2].yt_tStatePredicate)->formula); // d = (p & -q)
		AtomicBooleanPredicate * e = new AtomicBooleanPredicate(true);
		e -> addSub(c);
		e -> addSub((yyvsp[0].yt_tStatePredicate)->formula); // d = (p & -q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub(e); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if(((yyvsp[-2].yt_tStatePredicate) -> shape == AT_AND) && ((yyvsp[0].yt_tStatePredicate) -> shape == AT_OR))
	{
		StatePredicate * b = (yyvsp[0].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_AND
		StatePredicate * c = (yyvsp[-2].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_OR
		reinterpret_cast<AtomicBooleanPredicate *>(b) -> addSub((yyvsp[-2].yt_tStatePredicate)->formula); // p & -q
		AtomicBooleanPredicate * e = new AtomicBooleanPredicate(true);
		e -> addSub(c);
		e -> addSub((yyvsp[0].yt_tStatePredicate)->formula); // e = (-p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(b);
		a -> addSub(e); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if(((yyvsp[-2].yt_tStatePredicate) -> shape == AT_OR) && ((yyvsp[0].yt_tStatePredicate) -> shape == AT_AND))
	{
		StatePredicate * b = (yyvsp[-2].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_AND
		StatePredicate * c = (yyvsp[0].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_OR
		reinterpret_cast<AtomicBooleanPredicate *>(b) -> addSub((yyvsp[0].yt_tStatePredicate)->formula); // -p & q
		AtomicBooleanPredicate * e = new AtomicBooleanPredicate(true);
		e -> addSub(c);
		e -> addSub((yyvsp[-2].yt_tStatePredicate)->formula); // e = (p & -q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(b);
		a -> addSub(e); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else // (($1 -> shape == AT_OR) && ($3 -> shape == AT_OR))
	{
		StatePredicate * b = (yyvsp[-2].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_AND
		StatePredicate * c = (yyvsp[0].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_AND
		reinterpret_cast<AtomicBooleanPredicate *>(b) -> addSub((yyvsp[0].yt_tStatePredicate)->formula); // -p & q
		reinterpret_cast<AtomicBooleanPredicate *>(c) -> addSub((yyvsp[-2].yt_tStatePredicate)->formula); // p & -q
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(b);
		a -> addSub(c); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
    }
#line 1930 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 15:
#line 504 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_TEMP || (yyvsp[0].yt_tStatePredicate) -> shape == AT_TEMP)
	{
		(yyval.yt_tStatePredicate) = Implication((yyvsp[-2].yt_tStatePredicate), (yyvsp[0].yt_tStatePredicate)); 
		(yyval.yt_tStatePredicate) -> shape = AT_TEMP;
	}
	else if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_FALSE || (yyvsp[0].yt_tStatePredicate) -> shape == AT_TRUE)
	{
		delete (yyvsp[-2].yt_tStatePredicate) -> formula;
		delete (yyvsp[0].yt_tStatePredicate) -> formula;
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = new TruePredicate();
		(yyval.yt_tStatePredicate) -> shape = AT_TRUE;
	}
	else if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_TRUE)
	{
		(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> shape = (yyvsp[0].yt_tStatePredicate) -> shape;
	}
	else if((yyvsp[0].yt_tStatePredicate) -> shape == AT_FALSE)
	{
		(yyvsp[-2].yt_tStatePredicate)->formula = (yyvsp[-2].yt_tStatePredicate) -> formula -> negate();
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> shape = oppShape((yyvsp[-2].yt_tStatePredicate) -> shape);
	} 
	else if(((yyvsp[-2].yt_tStatePredicate)->shape == AT_AND) && (((yyvsp[0].yt_tStatePredicate) -> shape == AT_AND) || ((yyvsp[0].yt_tStatePredicate) -> shape == AT_COMP)|| ((yyvsp[0].yt_tStatePredicate)->shape == AT_FIR) || ((yyvsp[0].yt_tStatePredicate)->shape == AT_DL)))
	{
		 (yyvsp[-2].yt_tStatePredicate) -> formula = (yyvsp[-2].yt_tStatePredicate) -> formula ->negate();
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[-2].yt_tStatePredicate) -> formula) -> addSub((yyvsp[0].yt_tStatePredicate)->formula);
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if(((yyvsp[0].yt_tStatePredicate)->shape == AT_OR) && (((yyvsp[-2].yt_tStatePredicate) -> shape == AT_AND) || ((yyvsp[-2].yt_tStatePredicate) -> shape == AT_COMP)||((yyvsp[-2].yt_tStatePredicate)->shape == AT_FIR) || ((yyvsp[-2].yt_tStatePredicate)->shape == AT_DL)))
	{
		(yyvsp[-2].yt_tStatePredicate) -> formula = (yyvsp[-2].yt_tStatePredicate)-> formula -> negate();
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[0].yt_tStatePredicate) -> formula) -> addSub((yyvsp[-2].yt_tStatePredicate)->formula);
		(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
	}
	else if(((yyvsp[-2].yt_tStatePredicate)->shape == AT_AND) && ((yyvsp[0].yt_tStatePredicate) -> shape == AT_OR))
	{
		(yyvsp[-2].yt_tStatePredicate) -> formula = (yyvsp[-2].yt_tStatePredicate) -> formula -> negate();
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[0].yt_tStatePredicate) -> formula)->merge(reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[-2].yt_tStatePredicate)->formula));
		(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
	}
	else // both $1 and $3 are AT_* or AT_OR
	{
		(yyvsp[-2].yt_tStatePredicate) -> formula = (yyvsp[-2].yt_tStatePredicate) -> formula -> negate();
		AtomicBooleanPredicate * result = new AtomicBooleanPredicate(false);
		result -> addSub((yyvsp[-2].yt_tStatePredicate)->formula);
		result -> addSub((yyvsp[0].yt_tStatePredicate)->formula);
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = result;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
    }
#line 1990 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 16:
#line 560 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	// translate into (p & q) | (-p & -q)

	if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_TEMP || (yyvsp[0].yt_tStatePredicate) -> shape == AT_TEMP)
	{
		(yyval.yt_tStatePredicate) = Equivalence((yyvsp[-2].yt_tStatePredicate), (yyvsp[0].yt_tStatePredicate)); 
		(yyval.yt_tStatePredicate) -> shape = AT_TEMP;
	}
	else if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_FALSE)
	{
		(yyvsp[0].yt_tStatePredicate) -> formula = (yyvsp[0].yt_tStatePredicate) -> formula -> negate();
		(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> shape = oppShape((yyvsp[0].yt_tStatePredicate)->shape);
	}
	else if((yyvsp[0].yt_tStatePredicate) -> shape == AT_FALSE)
	{
		(yyvsp[-2].yt_tStatePredicate) -> formula = (yyvsp[-2].yt_tStatePredicate) -> formula -> negate();
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> shape = oppShape((yyvsp[-2].yt_tStatePredicate)->shape);
	}
	else if((yyvsp[0].yt_tStatePredicate) -> shape == AT_TRUE)
	{
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
	}
	else if((yyvsp[-2].yt_tStatePredicate) -> shape == AT_TRUE)
	{
		(yyval.yt_tStatePredicate) = (yyvsp[0].yt_tStatePredicate);
	}
	else if(((yyvsp[-2].yt_tStatePredicate) -> shape == AT_AND) && (((yyvsp[0].yt_tStatePredicate) -> shape == AT_COMP)||((yyvsp[0].yt_tStatePredicate)->shape == AT_FIR) || ((yyvsp[0].yt_tStatePredicate)->shape == AT_DL)))
	{
		StatePredicate * b = (yyvsp[0].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_ELEM
		StatePredicate * c = (yyvsp[-2].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_OR
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(c);
		d -> addSub(b); // d = (-p & -q)
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[-2].yt_tStatePredicate) -> formula) ->addSub((yyvsp[0].yt_tStatePredicate)->formula);     // $1 = (p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub((yyvsp[-2].yt_tStatePredicate)->formula);
		a -> addSub(d); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if((((yyvsp[-2].yt_tStatePredicate) -> shape == AT_COMP)|| ((yyvsp[-2].yt_tStatePredicate)->shape == AT_FIR) || ((yyvsp[-2].yt_tStatePredicate)->shape == AT_DL)) && ((yyvsp[0].yt_tStatePredicate) -> shape == AT_AND))
	{
		StatePredicate * b = (yyvsp[-2].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_ELEM
		StatePredicate * c = (yyvsp[0].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_OR
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(c);
		d -> addSub(b); // d = (-p & -q)
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[0].yt_tStatePredicate) -> formula) ->addSub((yyvsp[-2].yt_tStatePredicate)->formula);     // $3 = (p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub((yyvsp[0].yt_tStatePredicate)->formula); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if((((yyvsp[-2].yt_tStatePredicate) -> shape == AT_COMP)||((yyvsp[-2].yt_tStatePredicate)->shape == AT_FIR) || ((yyvsp[-2].yt_tStatePredicate)->shape == AT_DL)) && ((yyvsp[0].yt_tStatePredicate) -> shape == AT_OR))
	{
		StatePredicate * b = (yyvsp[-2].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_ELEM
		StatePredicate * c = (yyvsp[0].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_AND
		reinterpret_cast<AtomicBooleanPredicate *>(c) -> addSub(b); // c = -p & -q
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub((yyvsp[-2].yt_tStatePredicate)->formula);
		d -> addSub((yyvsp[0].yt_tStatePredicate)->formula); // d = (p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub(c); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if(((yyvsp[-2].yt_tStatePredicate) -> shape == AT_OR) && (((yyvsp[0].yt_tStatePredicate) -> shape == AT_COMP)||((yyvsp[0].yt_tStatePredicate)->shape==AT_FIR) || ((yyvsp[0].yt_tStatePredicate)->shape == AT_DL)))
	{
		StatePredicate * b = (yyvsp[0].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_ELEM
		StatePredicate * c = (yyvsp[-2].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_AND
		reinterpret_cast<AtomicBooleanPredicate *>(c) -> addSub(b); // c = -p & -q
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub((yyvsp[0].yt_tStatePredicate)->formula);
		d -> addSub((yyvsp[-2].yt_tStatePredicate)->formula); // d = (p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub(c); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if((((yyvsp[-2].yt_tStatePredicate) -> shape == AT_COMP)|| ((yyvsp[-2].yt_tStatePredicate)->shape == AT_FIR) || ((yyvsp[-2].yt_tStatePredicate)->shape == AT_DL)) && (((yyvsp[0].yt_tStatePredicate) -> shape == AT_COMP)||((yyvsp[0].yt_tStatePredicate)->shape == AT_FIR)||((yyvsp[0].yt_tStatePredicate)->shape == AT_DL)))
	{
		StatePredicate * b = (yyvsp[0].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_ELEM
		StatePredicate * c = (yyvsp[-2].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_AND
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(b);
		d -> addSub(c); // d = (-p & -q)
		AtomicBooleanPredicate * e = new AtomicBooleanPredicate(true);
		e -> addSub((yyvsp[-2].yt_tStatePredicate)->formula);
		e -> addSub((yyvsp[0].yt_tStatePredicate)->formula); // e = (p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub(e); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if(((yyvsp[-2].yt_tStatePredicate) -> shape == AT_AND) && ((yyvsp[0].yt_tStatePredicate) -> shape == AT_AND))
	{
		StatePredicate * b = (yyvsp[0].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_OR
		StatePredicate * c = (yyvsp[-2].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_OR
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(b);
		d -> addSub(c); // d = (-p & -q)
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[-2].yt_tStatePredicate)->formula)->merge(reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[0].yt_tStatePredicate)->formula)); //$1 = p&q
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub((yyvsp[-2].yt_tStatePredicate)->formula); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if(((yyvsp[-2].yt_tStatePredicate) -> shape == AT_AND) && ((yyvsp[0].yt_tStatePredicate) -> shape == AT_OR))
	{
		StatePredicate * b = (yyvsp[0].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_AND
		StatePredicate * c = (yyvsp[-2].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_OR
		reinterpret_cast<AtomicBooleanPredicate *>(b) -> addSub(c); // -p & -q
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[-2].yt_tStatePredicate)->formula)->addSub((yyvsp[0].yt_tStatePredicate)->formula); // $1 = p&q
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(b);
		a -> addSub((yyvsp[-2].yt_tStatePredicate)->formula); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else if(((yyvsp[-2].yt_tStatePredicate) -> shape == AT_OR) && ((yyvsp[0].yt_tStatePredicate) -> shape == AT_AND))
	{
		StatePredicate * b = (yyvsp[-2].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_AND
		StatePredicate * c = (yyvsp[0].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_OR
		reinterpret_cast<AtomicBooleanPredicate *>(b) -> addSub(c); // -p & -q
		reinterpret_cast<AtomicBooleanPredicate *>((yyvsp[0].yt_tStatePredicate)->formula)->addSub((yyvsp[-2].yt_tStatePredicate)->formula); //$3 = p&q
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(b);
		a -> addSub((yyvsp[0].yt_tStatePredicate)->formula); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
	else // (($1 -> shape == AT_OR) && ($3 -> shape == AT_OR))
	{
		StatePredicate * b = (yyvsp[-2].yt_tStatePredicate) -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_AND
		StatePredicate * c = (yyvsp[0].yt_tStatePredicate)->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_AND
		reinterpret_cast<AtomicBooleanPredicate *>(b) -> merge(reinterpret_cast<AtomicBooleanPredicate *>(c)); // -p & -q
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub((yyvsp[-2].yt_tStatePredicate)->formula);
		d -> addSub((yyvsp[0].yt_tStatePredicate)->formula); // d = p&q
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(b);
		a -> addSub(d); // a = result
		(yyval.yt_tStatePredicate) = (yyvsp[-2].yt_tStatePredicate);
		(yyval.yt_tStatePredicate) -> formula = a;
		(yyval.yt_tStatePredicate) -> shape = AT_OR;
	}
    }
#line 2175 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 17:
#line 741 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tStatePredicate) = AllPath((yyvsp[0].yt_tStatePredicate)); }
#line 2181 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 18:
#line 743 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tStatePredicate) = ExPath((yyvsp[0].yt_tStatePredicate)); }
#line 2187 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 745 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tStatePredicate) = Always((yyvsp[0].yt_tStatePredicate)); }
#line 2193 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 20:
#line 747 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tStatePredicate) = Eventually((yyvsp[0].yt_tStatePredicate)); }
#line 2199 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 21:
#line 749 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    {
        kc::tStatePredicate result = (yyvsp[0].yt_tStatePredicate);
        std::string op((yyvsp[-1].yt_casestring)->name);
        for (int i = op.size()-1; i >= 0; i--)
        {
            if (op[i] == 'A') result = AllPath(result);
            if (op[i] == 'E') result = ExPath(result);
            if (op[i] == 'F') result = Eventually(result);
            if (op[i] == 'G') result = Always(result);
            if (op[i] == 'X') result = NextState(result);
            if (op[i] == 'U') yyerrors((yyvsp[-1].yt_casestring)->name, (yylsp[-1]), "operator 'U' is not allowed here");
            if (op[i] == 'R') yyerrors((yyvsp[-1].yt_casestring)->name, (yylsp[-1]), "operator 'R' is not allowed here");
        }
        (yyval.yt_tStatePredicate) = result;
    }
#line 2219 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 22:
#line 765 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tStatePredicate) = Until((yyvsp[-3].yt_tStatePredicate), (yyvsp[-1].yt_tStatePredicate)); }
#line 2225 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 23:
#line 767 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tStatePredicate) = Release((yyvsp[-3].yt_tStatePredicate), (yyvsp[-1].yt_tStatePredicate)); }
#line 2231 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 24:
#line 769 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    {
        std::string op((yyvsp[-2].yt_casestring)->name);
        if (op == "R") {
            (yyval.yt_tStatePredicate) = Release((yyvsp[-3].yt_tStatePredicate), (yyvsp[-1].yt_tStatePredicate));
        } else if (op == "U") {
            (yyval.yt_tStatePredicate) = Until((yyvsp[-3].yt_tStatePredicate), (yyvsp[-1].yt_tStatePredicate));
        } else {
            yyerrors((yyvsp[-2].yt_casestring)->name, (yylsp[-2]), "operator '%s' is not allowed here", (yyvsp[-2].yt_casestring)->name);
        }
    }
#line 2246 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 25:
#line 780 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tStatePredicate) = NextState((yyvsp[0].yt_tStatePredicate)); }
#line 2252 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 26:
#line 782 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tStatePredicate) = ExPath(Eventually((yyvsp[0].yt_tStatePredicate))); }
#line 2258 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 27:
#line 784 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tStatePredicate) = AllPath(Always((yyvsp[0].yt_tStatePredicate))); }
#line 2264 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 28:
#line 786 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	(yyval.yt_tStatePredicate) = AllPath(Always(Negation((yyvsp[0].yt_tStatePredicate)))); 
    }
#line 2272 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 29:
#line 793 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	(yyvsp[0].yt_tTerm) -> ttt -> multiply(-1);
	(yyvsp[-2].yt_tTerm) -> ttt -> append((yyvsp[0].yt_tTerm) -> ttt);
	Term * T = (yyvsp[-2].yt_tTerm) -> ttt -> copy();
	T -> multiply(-1);
	AtomicBooleanPredicate * result = new AtomicBooleanPredicate(true);
	result -> addSub(new AtomicStatePredicate((yyvsp[-2].yt_tTerm)->ttt));	
	result -> addSub(new AtomicStatePredicate(T));	
	(yyval.yt_tAtomicProposition) = Elementary();
	(yyval.yt_tAtomicProposition) -> shape = AT_AND;
	(yyval.yt_tAtomicProposition) -> pred = result;
    }
#line 2289 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 30:
#line 806 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	(yyvsp[0].yt_tTerm) -> ttt -> multiply(-1);
	(yyvsp[-2].yt_tTerm) -> ttt -> append((yyvsp[0].yt_tTerm) -> ttt);
	Term * T = (yyvsp[-2].yt_tTerm) -> ttt -> copy();
	T -> multiply(-1);
	AtomicBooleanPredicate * result = new AtomicBooleanPredicate(true);
	result -> addSub(new AtomicStatePredicate((yyvsp[-2].yt_tTerm)->ttt));	
	result -> addSub(new AtomicStatePredicate(T));	
	result = (AtomicBooleanPredicate *) result -> negate();
	(yyval.yt_tAtomicProposition) = Elementary();
	(yyval.yt_tAtomicProposition) -> shape = AT_OR;
	(yyval.yt_tAtomicProposition) -> pred = result;
    }
#line 2307 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 31:
#line 820 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	(yyvsp[-2].yt_tTerm) -> ttt -> multiply(-1);
	(yyvsp[-2].yt_tTerm) -> ttt -> append((yyvsp[0].yt_tTerm) -> ttt);
	Term * T = new Term();
	T -> place = Net::Card[PL];
	T -> mult = 1;
	(yyvsp[-2].yt_tTerm) -> ttt -> append(T);
	AtomicStatePredicate * result = new AtomicStatePredicate((yyvsp[-2].yt_tTerm)->ttt);
	(yyval.yt_tAtomicProposition) = Elementary();
	(yyval.yt_tAtomicProposition) -> shape = AT_COMP;
	(yyval.yt_tAtomicProposition) -> pred = result;
	
    }
#line 2325 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 32:
#line 834 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	(yyvsp[-2].yt_tTerm) -> ttt -> multiply(-1);
	(yyvsp[-2].yt_tTerm) -> ttt -> append((yyvsp[0].yt_tTerm) -> ttt);
	AtomicStatePredicate * result = new AtomicStatePredicate((yyvsp[-2].yt_tTerm)->ttt);
	(yyval.yt_tAtomicProposition) = Elementary();
	(yyval.yt_tAtomicProposition) -> shape = AT_COMP;
	(yyval.yt_tAtomicProposition) -> pred = result;
    }
#line 2338 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 33:
#line 843 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	(yyvsp[0].yt_tTerm) -> ttt -> multiply(-1);
	(yyvsp[-2].yt_tTerm) -> ttt -> append((yyvsp[0].yt_tTerm) -> ttt);
	Term * T = new Term();
	T -> place = Net::Card[PL];
	T -> mult = 1;
	(yyvsp[-2].yt_tTerm) -> ttt -> append(T);
	AtomicStatePredicate * result = new AtomicStatePredicate((yyvsp[-2].yt_tTerm)->ttt);
	(yyval.yt_tAtomicProposition) = Elementary();
	(yyval.yt_tAtomicProposition) -> shape = AT_COMP;
	(yyval.yt_tAtomicProposition) -> pred = result;
    }
#line 2355 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 34:
#line 856 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	(yyvsp[0].yt_tTerm) -> ttt -> multiply(-1);
	(yyvsp[-2].yt_tTerm) -> ttt -> append((yyvsp[0].yt_tTerm) -> ttt);
	AtomicStatePredicate * result = new AtomicStatePredicate((yyvsp[-2].yt_tTerm)->ttt);
	(yyval.yt_tAtomicProposition) = Elementary();
	(yyval.yt_tAtomicProposition) -> shape = AT_COMP;
	(yyval.yt_tAtomicProposition) -> pred = result;
    }
#line 2368 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 35:
#line 865 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	TruePredicate * result = new TruePredicate();
	(yyval.yt_tAtomicProposition) = Elementary();
	(yyval.yt_tAtomicProposition) -> shape = AT_TRUE;
	(yyval.yt_tAtomicProposition) -> pred = result;
    }
#line 2379 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 36:
#line 872 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	FalsePredicate * result = new FalsePredicate();
	(yyval.yt_tAtomicProposition) = Elementary();
	(yyval.yt_tAtomicProposition) -> shape = AT_FALSE;
	(yyval.yt_tAtomicProposition) -> pred = result;
    }
#line 2390 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 37:
#line 879 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    {
        Symbol *t = symbolTables->TransitionTable->lookup((yyvsp[-1].yt_casestring)->name);
        if (UNLIKELY(t == NULL))
        {
            yyerrors((yyvsp[-1].yt_casestring)->name, (yylsp[-1]), "transition '%s' unknown", (yyvsp[-1].yt_casestring)->name);
        }
	FireablePredicate * result = new FireablePredicate(t->getIndex(),true);
        target_transition.insert(t->getIndex());
	(yyval.yt_tAtomicProposition) = Elementary();
	(yyval.yt_tAtomicProposition) -> shape = AT_FIR;
	(yyval.yt_tAtomicProposition) -> pred = result;
    }
#line 2407 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 38:
#line 892 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	AtomicBooleanPredicate * result = new AtomicBooleanPredicate(true);
	for(arrayindex_t i = 0; i < Place::CardSignificant;i++)
	{
		capacity_t m0 = Marking::Initial[i];
	
		// insert p <= m0
		Term * T1 = new Term();
		T1 -> place  = i;
		T1 -> mult = 1;
		Term * T2 = new Term();
		T2 -> place = Net::Card[PL];
		T2 -> mult = -m0;
		T1 -> append(T2);
		result -> addSub(new AtomicStatePredicate(T1));

		// insert p >= m0
		T1 = new Term();
		T1 -> place = i;
		T1 -> mult = -1;
		T2 = new Term();
		T2 -> place = Net::Card[PL];
		T2 -> mult = m0;
		T1 -> append(T2);
		result -> addSub(new AtomicStatePredicate(T1));
	}
	(yyval.yt_tAtomicProposition) = Elementary(); 
	(yyval.yt_tAtomicProposition) -> shape = AT_AND;
	(yyval.yt_tAtomicProposition) -> pred = result;
    }
#line 2442 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 39:
#line 923 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	DeadlockPredicate * result = new DeadlockPredicate(true);
	(yyval.yt_tAtomicProposition) = Elementary(); 
	(yyval.yt_tAtomicProposition) -> shape = AT_DL;
	(yyval.yt_tAtomicProposition) -> pred = result;
    }
#line 2453 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 40:
#line 933 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    {
		(yyval.yt_tFormula) = CompBound();
		(yyval.yt_tFormula) -> formula = new AtomicStatePredicate((yyvsp[-1].yt_tTerm)->ttt);
	}
#line 2462 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 41:
#line 940 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_tTerm) = (yyvsp[-1].yt_tTerm); }
#line 2468 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 42:
#line 942 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    {
        Symbol *p = symbolTables->PlaceTable->lookup((yyvsp[0].yt_casestring)->name);
        if (UNLIKELY(p == NULL))
        {
            yyerrors((yyvsp[0].yt_casestring)->name, (yylsp[0]), "place '%s' unknown", (yyvsp[0].yt_casestring)->name);
        }
	(yyval.yt_tTerm) = Complex();
	(yyval.yt_tTerm) -> ttt = new Term();
	(yyval.yt_tTerm) -> ttt -> place = p -> getIndex();
	(yyval.yt_tTerm) -> ttt -> mult = 1;
        target_place.insert(p->getIndex());
#ifdef RERS
	rers_place[p->getIndex()] = true;
#endif
    }
#line 2488 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 43:
#line 958 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	(yyval.yt_tTerm) = Complex(); 
	(yyval.yt_tTerm) -> ttt = new Term();
	(yyval.yt_tTerm) -> ttt -> place = Net::Card[PL];
	(yyval.yt_tTerm) -> ttt -> mult = (yyvsp[0].yt_integer) -> value;
    }
#line 2499 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 44:
#line 965 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	(yyvsp[-2].yt_tTerm) -> ttt -> append((yyvsp[0].yt_tTerm) -> ttt);
	(yyval.yt_tTerm) = (yyvsp[-2].yt_tTerm); 
    }
#line 2508 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 45:
#line 970 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	(yyvsp[0].yt_tTerm) -> ttt -> multiply(-1);
	(yyvsp[-2].yt_tTerm) -> ttt -> append((yyvsp[0].yt_tTerm) -> ttt);
	(yyval.yt_tTerm) = (yyvsp[-2].yt_tTerm); 
    }
#line 2518 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 46:
#line 976 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
	(yyvsp[0].yt_tTerm) -> ttt->multiply((yyvsp[-2].yt_integer)->value);
	(yyval.yt_tTerm) = (yyvsp[0].yt_tTerm); 
    }
#line 2527 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 47:
#line 981 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { 
      (yyval.yt_tTerm) = Complex();
      (yyval.yt_tTerm) -> ttt = new Term();
      (yyval.yt_tTerm) -> ttt -> place = Net::Card[PL];
      (yyval.yt_tTerm) -> ttt -> mult = OMEGA;
      if (RT::args.search_arg != search_arg_covergraph)
      {
          RT::rep->message("%s: omega markings used without %s",
              RT::rep->markup(MARKUP_WARNING, "warning").str(),
              RT::rep->markup(MARKUP_PARAMETER, "--search=cover").str());
      }
    }
#line 2544 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 48:
#line 996 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_casestring) = (yyvsp[0].yt_casestring); }
#line 2550 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;

  case 49:
#line 997 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1646  */
    { (yyval.yt_casestring) = (yyvsp[0].yt_casestring); }
#line 2556 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
    break;


#line 2560 "Frontend/Parser/ParserFormula.cc" /* yacc.c:1646  */
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
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

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
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
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

  yyerror_range[1] = yylloc;

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
                      yytoken, &yylval, &yylloc);
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
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

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

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

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp);
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
#line 1000 "Frontend/Parser/ParserFormula.yy" /* yacc.c:1906  */


/// display a parser error and exit
void ptformula_error(char const* mess) __attribute__((noreturn));
void ptformula_error(char const* mess)
{
    extern char* ptformula_text;  ///< the current token text from Flex
    yyerrors(ptformula_text, ptformula_lloc, mess);
}

tShape oppShape(tShape s)
{
	switch(s)
	{
	case AT_COMP: return AT_COMP;
	case AT_FIR: return AT_FIR;
	case AT_DL: return AT_DL;
	case AT_TEMP: return AT_TEMP;
	case AT_AND: return AT_OR;
	case AT_OR: return AT_AND;
	case AT_TRUE: return AT_FALSE;
	case AT_FALSE: return AT_TRUE;
	}
}

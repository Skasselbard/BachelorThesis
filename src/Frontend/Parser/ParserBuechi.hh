/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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

#ifndef YY_PTBUECHI_FRONTEND_PARSER_PARSERBUECHI_HH_INCLUDED
# define YY_PTBUECHI_FRONTEND_PARSER_PARSERBUECHI_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ptbuechi_debug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    END = 0,
    IDENTIFIER = 258,
    NUMBER = 259,
    _accept_ = 260,
    _buechi_ = 261,
    _braceleft_ = 262,
    _braceright_ = 263,
    _comma_ = 264,
    _then_ = 265,
    _colon_ = 266,
    _INITIAL_ = 267,
    _DEADLOCK_ = 268,
    _AND_ = 269,
    _NOT_ = 270,
    _OR_ = 271,
    _XOR_ = 272,
    _iff_ = 273,
    _notequal_ = 274,
    _implies_ = 275,
    _equals_ = 276,
    _plus_ = 277,
    _minus_ = 278,
    _times_ = 279,
    _leftparenthesis_ = 280,
    _rightparenthesis_ = 281,
    _greaterthan_ = 282,
    _lessthan_ = 283,
    _greaterorequal_ = 284,
    _lessorequal_ = 285,
    _semicolon_ = 286,
    _TRUE_ = 287,
    _FALSE_ = 288,
    _FIREABLE_ = 289
  };
#endif
/* Tokens.  */
#define END 0
#define IDENTIFIER 258
#define NUMBER 259
#define _accept_ 260
#define _buechi_ 261
#define _braceleft_ 262
#define _braceright_ 263
#define _comma_ 264
#define _then_ 265
#define _colon_ 266
#define _INITIAL_ 267
#define _DEADLOCK_ 268
#define _AND_ 269
#define _NOT_ 270
#define _OR_ 271
#define _XOR_ 272
#define _iff_ 273
#define _notequal_ 274
#define _implies_ 275
#define _equals_ 276
#define _plus_ 277
#define _minus_ 278
#define _times_ 279
#define _leftparenthesis_ 280
#define _rightparenthesis_ 281
#define _greaterthan_ 282
#define _lessthan_ 283
#define _greaterorequal_ 284
#define _lessorequal_ 285
#define _semicolon_ 286
#define _TRUE_ 287
#define _FALSE_ 288
#define _FIREABLE_ 289

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


extern YYSTYPE ptbuechi_lval;
extern YYLTYPE ptbuechi_lloc;
int ptbuechi_parse (void);

#endif /* !YY_PTBUECHI_FRONTEND_PARSER_PARSERBUECHI_HH_INCLUDED  */

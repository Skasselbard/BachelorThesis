%option noyywrap
%option outfile="scanner.cc" header-file="scanner.h"
%option yylineno

%{
#include <string>
#include "k.h"
#include "yystype.h"
#include "parser.h"
#include "diagnosis.h"

int yycolumn = 1;
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
  yylloc.first_column = yycolumn; yylloc.last_column = yycolumn+yyleng-1; \
  yycolumn += yyleng;
%}

%x COMMENT

%%

"/*"                           { BEGIN(COMMENT); }
<COMMENT>"*/"                  { BEGIN(INITIAL); }
<COMMENT>[^*\n\r]+             { /* ignore comments */ }
<COMMENT>"*"                   { /* ignore comments */ }
<COMMENT>[\n\r]                { /* ignore comments */ }
"//"[^\n\r]*"\n"               { /* comments */}

CONSUME                         { return _CONSUME_; }
FAIR                            { return _FAIR_; }
GUARD                          	{ return _GUARD_; }
MARKING                         { return _MARKING_; }
NET	                        { return _NET_; }
PLACE                           { return _PLACE_; }
PRODUCE                         { return _PRODUCE_; }
SAFE                            { return _SAFE_; }
STRONG                         	{ return _STRONG_; }
TRANSITION                     	{ return _TRANSITION_; }
WEAK                            { return _WEAK_; }

all				{ return _ALL_; }
bool	                        { return _BOOLEAN_; }
break	                        { return _BREAK_; }
case                            { return _CASE_; }
const                         	{ return _CONST_; }
continue                        { return _CONTINUE_; }
default                         { return _DEFAULT_; }
do                             	{ return _DO_; }
else                           	{ return _ELSE_; }
enum	                        { return _ENUM_; }
false                          	{ return _FALSE_; }
for                            	{ return _FOR_; }
if                         	{ return _IF_; }
int                        	{ return _INT_; }
return                         	{ return _RETURN_; }
struct                          { return _STRUCT_; }
switch                         	{ return _SWITCH_; }
true                          	{ return _TRUE_; }
typedef				{ return _TYPEDEF_; }
while                          	{ return _WHILE_; }
								
"^"				{ return _multiset_; }
"="                             { return _equals_; }
"["                             { return _leftbracket_; }
"]"                             { return _rightbracket_; }
"."                             { return _dot_; }
"+"                             { return _plus_; }
"-"                             { return _minus_; }
"*"                             { return _times_; }
"/"                             { return _divide_; }
":"                             { return _colon_; }
";"                             { return _semicolon_; }
"("                             { return _leftparenthesis_; }
")"                             { return _rightparenthesis_; }
","                             { return _comma_; }
">"                           	{ return _greaterthan_; }
"<"                            	{ return _lessthan_; }
">="                            { return _greaterorequal_; }
"<="                           	{ return _lessorequal_; }

"!="                            { return _notequal_; }
"{"                             { return _braceleft_; }
"}"                             { return _braceright_; }
"+="				{ return _plusAssign_;}
"-="				{ return _minusAssign_;}		
"*="				{ return _timesAssign_;}
"/="				{ return _divideAssign_;}
"%="				{ return _modAssign_;}
"++"				{ return _increment_;}
"--"				{ return _decrement_;}
"%"				{ return _mod_;}
"!"				{ return _not_;}
"&&"				{ return _and_;}
"||"				{ return _or_;}
"=="				{ return _equivalent_;}
"?"				{ return _otherIf_;}

[\n\r]                          { /* whitespace */ }
[\t ]                          	{ /* whitespace */ }

[0-9]+                       	{ yylval.yt_integer = kc::mkinteger(atoi(yytext)); return NUMBER; }

[A-Za-z_][A-Za-z0-9_]*("<<"[^\<\>\t\n\r]*">>")?	{ yylval.yt_casestring = kc::mkcasestring(yytext); return IDENT; /* TODO: Moduswechsel?? Kontrolle von [^...], nur >> verbieten?, welche whitespaces? */ }

.                               { yyerror("lexical error"); }

%%

%{
#include "k.h"
#include "yystype.h"
#include <cassert>

#include "diagnosis.h"
#include "scanner.h"
#include "symboltable.h"
#include <unfold.h>

extern kc::net root;

%}


%union { /*TODO*/
    kc::casestring yt_casestring;
    int value;
    kc::type yt_type;
    URcList* rcl;
    UEnList* el;
    ULVal* lval;
    int* exl;
    UStatement* stm;
    case_list* cl;
    UFunction* fu;
    UExpression* ex;
    arc_list* al;
    formula* form;
    IdList* idl;
    UTermList* tlist;
    Place* pl;
    Transition* tr;
    fmode* fm;
    Symbol* sy;
}

%expect 1 // DanglingElse conflict
%output="parser.cc"
%defines 
%error-verbose
%locations

/* Schlüsselworte */ /* Trennzeichen */ /* Konstanten */

%token IDENT "identifier"
%token NUMBER "number"
%token _CONSUME_ "CONSUME"
%token _FAIR_ "FAIR"
%token _GUARD_ "GUARD"
%token _MARKING_ "MARKING"
%token _NET_ "NET"
%token _PLACE_ "PLACE"
%token _PRODUCE_ "PRODUCE"
%token _SAFE_ "SAFE"
%token _STRONG_ "STRONG"
%token _TRANSITION_ "TRANSITION"
%token _WEAK_ "WEAK"
%token _ALL_"all"
%token _BOOLEAN_ "bool"
%token _BREAK_ "break"
%token _CASE_ "case"
%token _CONST_ "const"
%token _CONTINUE_ "continue"
%token _DEFAULT_ "default"
%token _DO_ "do"
%token _ELSE_ "else"
%token _ENUM_ "enum"
%token _FALSE_ "false"
%token _FOR_ "for"
%token _IF_ "if"
%token _INT_ "int"
%token _RETURN_ "return"
%token _STRUCT_ "struct"
%token _SWITCH_ "switch"
%token _TRUE_ "true"
%token _TYPEDEF_ "typedef"
%token _WHILE_ "while"
%token _multiset_ "^"
%token _leftbracket_ "[" 
%token _rightbracket_ "]"
%token _dot_ "."
%token _colon_ ":"
%token _semicolon_ ";" 
%token _leftparenthesis_ "("
%token _rightparenthesis_ ")"
%token _comma_ ","
%token _braceleft_ "{"
%token _braceright_ "}"
%token _plusAssign_ "+="
%token _minusAssign_ "-="
%token _timesAssign_ "*="
%token _divideAssign_ "/="
%token _modAssign_ "%="
%token _increment_ "++"
%token _decrement_ "--"
%token _not_ "!"
%token _and_ "&&"
%token _or_ "||"
%token _equivalent_ "==" 
%token _otherIf_ "?"

/* Prioritäten, Assoziativitäten */

%left _comma_
%left _equals_ _notequal_
%right _otherIf_ _colon_
%left _lessthan_ _lessorequal_ _greaterthan_ _greaterorequal_
%left _plus_ _minus_ _times_ _divide_ _mod_ _and_ _or_
%right _not_

%left _plusAssign_ _minusAssign_ _timesAssign_ _divideAssign_ _modAssign_ _equivalent_

/* Terminale */

%type <yt_integer> NUMBER
%type <yt_casestring> IDENT
//%type <str> IDENT

/* Nichtterminale */

%type<yt_net> net
%type<yt_netName> netName
%type<yt_definitionsList> definitionsList
%type<yt_definitions> definitions 
%type<yt_sort> sort
%type<yt_type> ttype
%type<yt_integer> optionalNumber
%type<yt_identList> identList
%type<yt_idents> idents
%type<yt_structTypeList> structTypeList 
%type<yt_structType> structType
%type<yt_varOrArray> varOrArray
%type<yt_arrayList> arrayList
%type<yt_array> array
%type<yt_function> function
%type<yt_optFunctionParameters> optFunctionParameters
%type<yt_functionParametersList> functionParametersList
%type<yt_functionParameters> functionParameters
%type<yt_constant> constant
%type<yt_expression> expression
%type<yt_assignment> assignment
%type<yt_incdecrement> incdecrement
%type<yt_arithmetic> arithmetic
%type<yt_logical> logical
%type<yt_comparison> comparison
%type<yt_leftvalue> leftvalue
%type<yt_other> other
%type<yt_initializerList> initializerList
%type<yt_expressionListColon> expressionListColon
%type<yt_places> places
%type<yt_optSafe> optSafe
%type<yt_optNumber> optNumber
%type<yt_optType> optType
%type<yt_place> place
%type<yt_commaPlaces> commaPlaces
%type<yt_morePlaces> morePlaces
%type<yt_marking> marking
%type<yt_markingList> markingList
%type<yt_transition> transition
%type<yt_fairness> fairness
%type<yt_variable> variable
%type<yt_varOrArrayList> varOrArrayList
%type<yt_guard> guard
%type<yt_optIdentExprList> optIdentExprList
%type<yt_identExprList> identExprList
%type<yt_block> block
%type<yt_declarationOrStatement> declarationOrStatement
%type<yt_declaration> declaration
%type<yt_statement> statement
%type<yt_ifStatement> ifStatement
%type<yt_switchCase> switchCase
%type<yt_switchCaseList> switchCaseList
%type<yt_optElse> optElse
%type<yt_expressionWithoutInitializer> expressionWithoutInitializer
%type<yt_arithmeticWithoutInitializer> arithmeticWithoutInitializer
%type<yt_logicalWithoutInitializer> logicalWithoutInitializer
%type<yt_comparisonWithoutInitializer> comparisonWithoutInitializer
%type<yt_otherWithoutInitializer> otherWithoutInitializer

%%

net:
  { SymbolTable::pushScope(); }
  netName definitionsList _PLACE_ places _MARKING_ marking _semicolon_ transition 
  { SymbolTable::popScope(); }
  { root = $$ = kc::Net($2,$3,$5,$7,$9); } 
;

netName:
  /* empty */ { $$ = kc::EmptyNetName() ; }
| _NET_ IDENT { SymbolTable::define($2->name, MODULE, @2);}
  { $$ = kc::NetName($2) ; }
;

definitionsList:
  /* empty */ { $$ = kc::EmptyDefinitionsList() ; }
| definitionsList definitions { $$ = kc::DefinitionsList($1,$2) ; }
;

definitions:
  sort { $$ = kc::DefSort($1) ; }
| function { $$ = kc::DefFunction($1) ; }
| constant { $$ = kc::DefConstant($1) ; }
;

sort:
 _TYPEDEF_ ttype IDENT { SymbolTable::define($3->name, SORT, @3); } arrayList  _semicolon_ { 
	// create node in parse tree
	$$ = kc::Sort($2,$3,$5) ; 
	// check: type name already used
	SoSymbol* s;
        if ((s = (SoSymbol*)(GlobalTable->lookup($3->name)))) {
            //yyerrors($1, "sort symbol name '%s' already used", _cimportant_($1));
		yyerror("type name already used");
        }
	// insert type symbol
	// todo: snd argument = unparsed version of $$
        s = new SoSymbol($3->name, NULL);
}
;

ttype:
  _BOOLEAN_ { $$ = kc::TypeBool() ; 
              $$.isFinite = true; }
| _ENUM_ _braceleft_ identList _braceright_ 
  { $$ = kc::TypeEnum($3) ; 
    $$.isFinite = true;
    // todo: unparse numerical values
  }
| IDENT { SymbolTable::use($1->name, @1); } 
  { 
	// todo: return type info from symbol table
	// $$ = kc::TypeIdent($1) ; 
  }
| _INT_ _leftparenthesis_ expression _comma_ expression _rightparenthesis_ 
   { 
	// todo: check expression type
	// check const expressions
	$3 -> unparse(dummyprinter,constexpression);
	$5 -> unparse(dummyprinter,constexpression);
	if(!$3.isConst) 
 	{
		yyerror("left bound of bounded integer must be constant");
	}
	if(!$5.isConst) 
 	{
		yyerror("right bound of bounded integer must be constant");
	}
	if($3.constValue > $5.constValue) 
 	{
		yyerror("right bound of bounded integer must not be smaller than left bound");
	}
	// create type object
	$$ = kc::TypeIntInterval($3,$5) ; 
   }
| _STRUCT_ _braceleft_ structTypeList _braceright_ { $$ = kc::TypeStruct($3); }
| _INT_ { $$ = kc::TypeInt() ; }
| _multiset_ ttype 
   { 
	// todo: check finiteness of base type
	$$ = kc::TypeMultiset($2) ; 
   }
;

optionalNumber:
  /* empty */ { $$ = kc::mkinteger(-1) ; }
| _colon_ NUMBER { $$ = $2 ; }
;

identList:
  IDENT  optionalNumber 
	{ 
	       SymbolTable::define($1->name, ENUM, @1); 	
	       $$ = kc::IdentList(kc::EmptyIdentList(),kc::Ident($1,$2)); 
        }
| identList idents { $$ = kc::IdentList($1,$2) ; }
;

idents:
_comma_ IDENT  optionalNumber 
     { 
	 SymbolTable::define($2->name, ENUM, @2); 	
         $$ = kc::Ident($2,$3) ; 
     }
;

structTypeList:
  structType { $$ = kc::SingleStructTypeList($1) ; }
| structTypeList structType { $$ = kc::StructTypeList($1,$2) ; }
;

structType:
  ttype IDENT 
  { SymbolTable::define($2->name, STRUCTTYPE, @2); }
  arrayList _semicolon_ 
  { $$ = kc::StructType($1,$2,$4) ; }
;

varOrArray:
 IDENT { SymbolTable::define($1->name, VARIABLE, @1); } arrayList { $$ = kc::VarOrArray($1,$3) ; }
;

arrayList:
  /* empty */ { $$ = kc::EmptyArrayList() ; }
| arrayList array { $$ = kc::ArrayList($1,$2) ; }
;

array:
_leftbracket_ expression _rightbracket_ { $$ = kc::Array($2) ; }
;

function:
  ttype IDENT { SymbolTable::define($2->name, FUNCTION, @2); } arrayList
  { SymbolTable::pushScope(); }
  _leftparenthesis_ optFunctionParameters _rightparenthesis_ block 
  { SymbolTable::popScope(); }
  { $$ = kc::Function($1,$2,$4, $7,$9) ; }
;

optFunctionParameters:
  /* empty */ { $$ = kc::EmptyOptFunctionParameters() ; }
| ttype varOrArray functionParametersList { $$ = kc::OptFunctionParameters($1,$2,$3) ; }
;

functionParametersList:
  /* empty */ { $$ = kc::EmptyFunctionParametersList() ; }
| functionParametersList functionParameters { $$ = kc::FunctionParametersList($1,$2) ; }
;

functionParameters:
_comma_ ttype varOrArray { $$ = kc::FunctionParameters($2,$3) ; }
;

constant: 
_CONST_ ttype IDENT { SymbolTable::define($3->name, CONSTANT, @3); } arrayList _equals_ expression _semicolon_ { $$ = kc::Constant($2,$3,$5,$7) ; }
;

expression:
  assignment { $$ = kc::ExprAssignment($1) ; }
| incdecrement { $$ = kc::ExprIncdecrement($1) ; }
| arithmetic { $$ = kc::ExprArithmetic($1) ; }
| logical { $$ = kc::ExprLogical($1) ; /*assert($$->ttype_id==1);*/}
| comparison { $$ = kc::ExprComparison($1) ; }
| leftvalue { $$ = kc::ExprLeftvalue($1) ; }
| other { $$ = kc::ExprOther($1) ; }
| initializerList { $$ = kc::ExprInitializerList($1) ; }
| _leftparenthesis_ expression _rightparenthesis_ { $$ = kc::ExprInParenthesis($2) ; }
;

expressionWithoutInitializer:
  assignment { $$ = kc::ExprAssignment2($1) ; }
| incdecrement { $$ = kc::ExprIncdecrement2($1) ; }
| arithmeticWithoutInitializer { $$ = kc::ExprArithmeticWithoutInitializer($1) ; }
| logicalWithoutInitializer { $$ = kc::ExprLogicalWithoutInitializer($1) ; }
| comparisonWithoutInitializer { $$ = kc::ExprComparisonWithoutInitializer($1) ; }
| leftvalue { $$ = kc::ExprLeftvalue2($1) ; }
| otherWithoutInitializer { $$ = kc::ExprOtherWithoutInitializer($1) ; }
| _leftparenthesis_ expression _rightparenthesis_ { $$ = kc::ExprInParenthesis2($2) ; }
;

assignment: 
  leftvalue _equals_ expression { $$ = kc::AssignEqual($1,$3) ; }
| leftvalue _plusAssign_ expression { $$ = kc::AssignPlus($1,$3) ; }
| leftvalue _minusAssign_ expression { $$ = kc::AssignMinus($1,$3) ; }
| leftvalue _timesAssign_ expression { $$ = kc::AssignTimes($1,$3) ; }
| leftvalue _divideAssign_ expression { $$ = kc::AssignDivide($1,$3) ; }
| leftvalue _modAssign_ expression { $$ = kc::AssignMod($1,$3) ; }
;

incdecrement:
  _increment_ leftvalue { $$ = kc::IncrementVal($2) ; }
| _decrement_ leftvalue { $$ = kc::DecrementVal($2) ; }
| leftvalue _increment_ { $$ = kc::ValIncrement($1) ; }
| leftvalue _decrement_ { $$ = kc::ValDecrement($1) ; }
;

arithmetic:
  _plus_ expression { $$ = kc::PositiveExpr($2) ; }
| _minus_ expression { $$ = kc::NegativeExpr($2) ; }
| expression _plus_ expression { $$ = kc::ExprAddition($1,$3) ; }
| expression _minus_ expression { $$ = kc::ExprSubtraction($1,$3) ; }
| expression _times_ expression { $$ = kc::ExprMultiplication($1,$3) ; }
| expression _divide_ expression { $$ = kc::ExprDivision($1,$3) ; }
| expression _mod_ expression { $$ = kc::ExprModulo($1,$3) ; }
| NUMBER { $$ = kc::ExprNumber($1) ; }
;

arithmeticWithoutInitializer:
  _plus_ expression { $$ = kc::PositiveExpr2($2) ; }
| _minus_ expression { $$ = kc::NegativeExpr2($2) ; }
| expressionWithoutInitializer _plus_ expression { $$ = kc::ExprAdditionWithoutInitializer($1,$3) ; }
| expressionWithoutInitializer _minus_ expression { $$ = kc::ExprSubtractionWithoutInitializer($1,$3) ; }
| expressionWithoutInitializer _times_ expression { $$ = kc::ExprMultiplicationWithoutInitializer($1,$3) ; }
| expressionWithoutInitializer _divide_ expression { $$ = kc::ExprDivisionWithoutInitializer($1,$3) ; }
| expressionWithoutInitializer _mod_ expression { $$ = kc::ExprModuloWithoutInitializer($1,$3) ; }
| NUMBER { $$ = kc::ExprNumber2($1) ; }
;

logical:
  _not_ expression { $$ = kc::NotExpr($2) ; }
| expression _and_ expression { $$ = kc::ExprAnd($1,$3) ; }
| expression _or_ expression { $$ = kc::ExprOr($1,$3) ; }
| _TRUE_ { $$ = kc::ExprTrue() ;}
| _FALSE_ { $$ = kc::ExprFalse() ; }
;

logicalWithoutInitializer:
  _not_ expression { $$ = kc::NotExpr2($2) ; }
| expressionWithoutInitializer _and_ expression { $$ = kc::ExprAndWithoutInitializer($1,$3) ; }
| expressionWithoutInitializer _or_ expression { $$ = kc::ExprOrWithoutInitializer($1,$3) ; }
| _TRUE_ { $$ = kc::ExprTrue2() ; }
| _FALSE_ { $$ = kc::ExprFalse2() ; }
;

comparison:
  expression _equivalent_ expression { $$ = kc::ExprEquivalent($1,$3) ; }
| expression _notequal_ expression { $$ = kc::ExprNotEqual($1,$3) ; }
| expression _lessthan_ expression { $$ = kc::ExprLessThan($1,$3) ; }
| expression _greaterthan_ expression { $$ = kc::ExprGreaterThan($1,$3) ; }
| expression _lessorequal_ expression { $$ = kc::ExprLessOrEqual($1,$3) ; }
| expression _greaterorequal_ expression { $$ = kc::ExprGreaterOrEqual($1,$3) ; }
;

comparisonWithoutInitializer:
  expressionWithoutInitializer _equivalent_ expression { $$ = kc::ExprEquivalentWithoutInitializer($1,$3) ; }
| expressionWithoutInitializer _notequal_ expression { $$ = kc::ExprNotEqualWithoutInitializer($1,$3) ; }
| expressionWithoutInitializer _lessthan_ expression { $$ = kc::ExprLessThanWithoutInitializer($1,$3) ; }
| expressionWithoutInitializer _greaterthan_ expression { $$ = kc::ExprGreaterThanWithoutInitializer($1,$3) ; }
| expressionWithoutInitializer _lessorequal_ expression { $$ = kc::ExprLessOrEqualWithoutInitializer($1,$3) ; }
| expressionWithoutInitializer _greaterorequal_ expression { $$ = kc::ExprGreaterOrEqualWithoutInitializer($1,$3) ; }
;

leftvalue:
  IDENT { SymbolTable::use($1->name, @1); } { $$ = kc::LeftValIdent($1) ; }
| leftvalue _leftbracket_ expression _rightbracket_ { $$ = kc::LeftValBrackets($1,$3) ; }
| leftvalue _dot_ IDENT { SymbolTable::use($3->name, @3); } { $$ = kc::LeftValDot($1,$3) ; }
;

other:
  IDENT _leftparenthesis_ _rightparenthesis_ { SymbolTable::use($1->name, @1); } { $$ = kc::ExprIdentParenthesis($1) ; } 
| IDENT _leftparenthesis_ expression _rightparenthesis_ { SymbolTable::use($1->name, @1); } { $$ = kc::ExprIdentExprInParenthesis($1,$3) ; } 
| expression _comma_ expression { $$ = kc::ExprCommaSeparated($1,$3) ; }
| expression _otherIf_ expression _colon_ expression { $$ = kc::ExprOtherIf($1,$3,$5) ; }
| _ALL_ _leftparenthesis_ ttype _rightparenthesis_ { $$ = kc::ExprAll($3) ; }
;

otherWithoutInitializer:
  IDENT _leftparenthesis_ _rightparenthesis_ { SymbolTable::use($1->name, @1); } { $$ = kc::ExprIdentParenthesis2($1) ; } 
| IDENT _leftparenthesis_ expression _rightparenthesis_ { SymbolTable::use($1->name, @1); } { $$ = kc::ExprIdentExprInParenthesis2($1,$3); } 
| expressionWithoutInitializer _comma_ expression { $$ = kc::ExprCommaSeparatedWithoutInitializer($1,$3) ; }
| expressionWithoutInitializer _otherIf_ expression _colon_ expression { $$ = kc::ExprOtherIfWithoutInitializer($1,$3,$5) ; }
| _ALL_ _leftparenthesis_ ttype _rightparenthesis_ { $$ = kc::ExprAll2($3) ; }
;

initializerList:
  _braceleft_ _braceright_ { $$ = kc::Braces() ; }
| _braceleft_ expression _braceright_ { $$ = kc::ExprInBraces($2) ; }
| _braceleft_ expression _colon_ expression expressionListColon _braceright_ { $$ = kc::ExprInBracesColon($2,$4,$5) ; }
;

expressionListColon:
  /* empty */ { $$ = kc::EmptyExpressionListColon() ; }
| _comma_ expression _colon_ expression expressionListColon { $$ = kc::ExpressionListColon($2,$4,$5) ; }
;

places:
  optSafe optType place commaPlaces _semicolon_ morePlaces { $$ = kc::Places($1,$2,$3,$4,$6) ; }
;

optSafe:
  /* empty */ { $$ = kc::EmptySafe() ; }
| _SAFE_ optNumber { $$ = kc::Safe($2) ; }
;

optNumber:
  /*empty*/ { $$ = kc::EmptyPlaceNumber() ; }
| NUMBER { $$ = kc::PlaceNumber($1) ; }
;

optType:
  /* empty */ { $$ = kc::EmptyPlaceType() ; }
| _BOOLEAN_ { $$ = kc::PlaceTypeBool() ; }
| _ENUM_ _braceleft_ IDENT { SymbolTable::define($3->name, ENUM, @3); } optionalNumber identList _braceright_ { $$ = kc::PlaceTypeEnum($3,$5,$6) ; }
| _INT_ _leftparenthesis_ expression _comma_ expression _rightparenthesis_ { $$ = kc::PlaceTypeIntInterval($3,$5) ; }
| _STRUCT_ _braceleft_ structTypeList _braceright_ { $$ = kc::PlaceTypeStruct($3) ;}
| _INT_ { $$ = kc::PlaceTypeInt() ; }
| _multiset_ ttype { $$ = kc::PlaceTypeMultiset($2) ; }
;

place:
  IDENT { SymbolTable::define($1->name, PLACE, @1); } { $$ = kc::Place($1) ; }
| IDENT IDENT { SymbolTable::use($1->name, @1); SymbolTable::define($2->name, PLACE, @2); } { $$ = kc::PlaceWithTypeIdent($1,$2) ; }
  // wegen Konflikt ist "typeIdent placeIdent" hier ausgelagert
;

commaPlaces:
  /* empty */ { $$ = kc::EmptyCommaPlaces() ; }
| _comma_ optType place commaPlaces { $$ = kc::CommaPlaces($2,$3,$4) ; }
;

morePlaces:
  /* empty */ { $$ = kc::EmptyMorePlaces() ; }
| optSafe optType place commaPlaces _semicolon_ morePlaces { $$ = kc::MorePlaces($1,$2,$3,$4,$6) ; }
;

marking:
  /* empty */ { $$ = kc::EmptyMarking() ; }
| IDENT { SymbolTable::use($1->name, @1); } _colon_ expression markingList { $$ = kc::Marking($1,$4,$5) ; }
;

markingList:
  /* empty */ { $$ = kc::EmptyMarkingList() ; }
| _comma_ IDENT { SymbolTable::use($2->name, @2); } _colon_ expression markingList { $$ = kc::MarkingList($2,$5,$6) ; }
;

transition:
  /* empty */ { $$ = kc::EmptyTransition() ; }
| _TRANSITION_ IDENT { SymbolTable::define($2->name, TRANSITION, @2); } { SymbolTable::pushScope(); } fairness variable guard _CONSUME_ optIdentExprList _semicolon_ _PRODUCE_ optIdentExprList _semicolon_ { SymbolTable::popScope(); } transition { $$ = kc::Transition($2,$5,$6,$7,$9,$12,$15) ; }
;

fairness:
  /* empty */ { $$ = kc::EmptyFairness() ; }
| _WEAK_ _FAIR_ { $$ = kc::WeakFair() ; }
| _STRONG_ _FAIR_ { $$ = kc::StrongFair() ; }
;

variable:
  /* empty */ { $$ = kc::EmptyVariable() ; }
| ttype varOrArray varOrArrayList _semicolon_ variable { $$ = kc::Variable($1,$2,$3,$5) ; }
;

varOrArrayList:
  /* empty */ { $$ = kc::EmptyVarOrArrayList() ; }
| _comma_ varOrArray varOrArrayList { $$ = kc::VarOrArrayList($2,$3) ; }
;

guard:
  /* empty */ { $$ = kc::EmptyGuard() ; }
| _GUARD_ expression { $$ = kc::Guard($2) ; }
;

optIdentExprList:
  /* empty */ { $$ = kc::EmptyOptIdentExprList() ; }
| IDENT { SymbolTable::use($1->name, @1); } _colon_ expression identExprList { $$ = kc::OptIdentExprList($1,$4,$5) ; }
;

identExprList:
  /* empty */ { $$ = kc::EmptyIdentExprList() ; }
| _comma_ IDENT { SymbolTable::use($2->name, @2); } _colon_ expression identExprList { $$ = kc::IdentExprList($2,$5,$6) ; }
;

block:
  _braceleft_ declarationOrStatement _braceright_ { $$ = kc::Block($2) ; }
;

declarationOrStatement:
  /* empty */ { $$ = kc::EmptyDeclarationOrStatement() ; }
| declaration declarationOrStatement { $$ = kc::DeclOrStatemDeclaration($1,$2) ; }
| statement declarationOrStatement { $$ = kc::DeclOrStatemStatement($1,$2) ; }
; 

declaration: 
  ttype varOrArray varOrArrayList _semicolon_ { $$ = kc::Declaration($1,$2,$3) ; }
;

statement:
  _semicolon_ { $$ = kc::StatementSemiolon() ; }
| { SymbolTable::pushScope(); } block { SymbolTable::popScope(); } { $$ = kc::StatementBlock($2) ; }
| expressionWithoutInitializer _semicolon_ { $$ = kc::StatementExprSemicolon($1) ; }
| ifStatement { $$ = kc::StatementIf($1) ; }
| _WHILE_ _leftparenthesis_ expression _rightparenthesis_ statement { $$ = kc::StatementWhile($3,$5) ; }
| _DO_ statement _WHILE_ _leftparenthesis_ expression _rightparenthesis_ _semicolon_ { $$ = kc::StatementDoWhile($2,$5) ; }
| _FOR_ _leftparenthesis_ expression _semicolon_ expression _semicolon_ expression _rightparenthesis_ statement { $$ = kc::StatementForExpr($3,$5,$7,$9) ; }
| _FOR_ _leftparenthesis_ ttype IDENT { SymbolTable::define($4->name, VARIABLE, @4); } _equals_ expression _semicolon_ expression _semicolon_ expression _rightparenthesis_ statement { $$ = kc::StatementForTypeExpr($3,$4,$7,$9,$11,$13) ; }
| _FOR_ _leftparenthesis_ IDENT { SymbolTable::use($3->name, @3); } _colon_ expression _rightparenthesis_ statement { $$ = kc::StatementForIdentColon($3,$6,$8) ; }
| _FOR_ _leftparenthesis_ ttype IDENT { SymbolTable::define($4->name, VARIABLE, @4); } _colon_ _ALL_ _leftparenthesis_ ttype _rightparenthesis_ _rightparenthesis_ { $$ = kc::StatementForAll($3,$4,$9) ; }
| _SWITCH_ _leftparenthesis_ expression _rightparenthesis_ _braceleft_ switchCase _braceright_ { $$ = kc::StatementSwitch($3,$6) ; }
| _BREAK_ _semicolon_ { $$ = kc::StatementBreak() ; }
| _CONTINUE_ _semicolon_ { $$ = kc::StatementContinue() ; }
| _RETURN_ expression _semicolon_ { $$ = kc::StatementReturn($2) ; }
;

ifStatement:
  _IF_ _leftparenthesis_ expression _rightparenthesis_ statement optElse { $$ = kc::IfStatement($3,$5,$6) ; }

optElse:
  /*empty*/ { $$ = kc::OptElseEmpty() ; }
| _ELSE_ statement { $$ = kc::OptElse($2) ; }
;

switchCase:
  _CASE_ expression _colon_ statement switchCaseList { $$ = kc::SwitchCase($2,$4,$5) ; }
| _DEFAULT_ _colon_ statement switchCaseList { $$ = kc::SwitchDefault($3,$4) ; }
;

switchCaseList:
  /* empty */ { $$ = kc::EmptySwitchCaseList() ; }
| _CASE_ expression _colon_ statement switchCaseList { $$ = kc::SwitchCaseList($2,$4,$5) ; }
| _DEFAULT_ _colon_ statement switchCaseList { $$ = kc::SwitchDefaultList($3,$4) ; }
;

%%

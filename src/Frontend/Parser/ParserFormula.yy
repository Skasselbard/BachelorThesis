/****************************************************************************
  This file is part of LoLA.

  LoLA is free software: you can redistribute it and/or modify it under the
  terms of the GNU Affero General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option)
  any later version.

  LoLA is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
  more details.

  You should have received a copy of the GNU Affero General Public License
  along with LoLA. If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

/*!
\file
\brief formula syntax
\author <unknown>
\status new
\ingroup g_frontend

Parses a formula in LoLA syntax.
*/

%{
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
%}

%error-verbose /* more verbose and specific error message string */
%defines       /* write an output file containing macro definitions for the token types */
%name-prefix="ptformula_"
%locations     /* we want to use token locations for better error messages */

%type <yt_tFormula> compoundformula
%type <yt_tFormula> formula
%type <yt_tFormula> computeboundformula
%type <yt_tStatePredicate> statepredicate
%type <yt_tAtomicProposition> atomic_proposition
%type <yt_tTerm> term
%type <yt_integer> NUMBER
%type <yt_casestring> IDENTIFIER _CTLOPERATOR_ identifier

%token _CTLOPERATOR_       "CTL* operator"
%token IDENTIFIER          "identifier"
%token NUMBER              "number"
%token _RELEASE_           "temporal operator RELEASE"
%token _NEXTSTATE_         "temporal operator NEXTSTATE"
%token _INITIAL_           "keyword INITIAL"
%token _DEADLOCK_          "keyword DEADLOCK"
%token _FORMULA_           "keyword FORMULA"
%token _MAX_           	   "keyword MAX"
%token _AND_               "Boolean conjuction"
%token _NOT_               "Boolean negation"
%token _OR_                "Boolean disjunction"
%token _XOR_               "Boolean exclusive disjunction"
%token _iff_               "Boolean iff"
%token _ALLPATH_           "path quantifier ALLPATH"
%token _ALWAYS_            "temporal operator ALWAYS"
%token _EVENTUALLY_        "temporal operator EVENTUALLY"
%token _EXPATH_            "path quantifier EXPATH"
%token _UNTIL_             "temporal operator UNTIL"
%token _REACHABLE_         "keyword REACHABLE"
%token _INVARIANT_         "keyword INVARIANT"
%token _IMPOSSIBLE_        "keyword IMPOSSIBLE"
%token _notequal_          "not-equals sign"
%token _implies_           "Boolean implication"
%token _equals_            "equals sign"
%token _plus_              "plus sign"
%token _minus_             "minus sign"
%token _times_             "multiplication sign"
%token _leftparenthesis_   "opening parenthesis"
%token _rightparenthesis_  "closing parenthesis"
%token _greaterthan_       "greater-than sign"
%token _lessthan_          "less-than sign"
%token _greaterorequal_    "greater-than-or-equal sign"
%token _lessorequal_       "less-than-or-equal sign"
%token _semicolon_         "semicolon"
%token _TRUE_              "Boolean TRUE"
%token _FALSE_             "Boolean FALSE"
%token _FIREABLE_          "keyword FIREABLE"
%token _omega_             "omega"
%token _colon_             "colon"
%token END 0               "end of file"

// precedences (lowest written first, e.g. PLUS/MINUS) and precedences
%nonassoc _REACHABLE_ _INVARIANT_ _IMPOSSIBLE_
%left _iff_
%left _implies_
%left _OR_ _XOR_
%left _AND_
%right _NOT_
%left _ALLPATH_ _EXPATH_ _CTLOPERATOR_ _UNTIL_ _ALWAYS_ _EVENTUALLY_ _NEXTSTATE_
%nonassoc _lessthan_ _lessorequal_ _greaterthan_ _greaterorequal_  _equals_ _notequal_
%left _plus_ _minus_
%left _times_

%{
// parser essentials
extern int ptformula_lex();
void ptformula_error(char const*);


std::set<arrayindex_t> target_place;
std::set<arrayindex_t> target_transition;
%}

%{
/* globals */
tFormula TheFormula;
%}

%%

compoundformula:
     formula { $$ = TheFormula = $1;}
| compoundformula _colon_ formula {$$ = TheFormula = Compound($1,$3);}
;
formula:
  _FORMULA_ statepredicate _semicolon_
    { $$ = StatePredicateFormula($2); }
| _FORMULA_ statepredicate
    { $$ = StatePredicateFormula($2); }
| statepredicate
    { $$ = StatePredicateFormula($1); }
| statepredicate _semicolon_
    { $$ = StatePredicateFormula($1); }
| computeboundformula 
    { $$ = $1; }
;

statepredicate:
  _leftparenthesis_ statepredicate _rightparenthesis_
    { $$ = $2; }
| atomic_proposition
    { 
	if($1->pred->magicnumber == MAGIC_NUMBER_TRUE)
	{
		delete $1->pred;
		$1 -> pred = new TruePredicate();
	}
	else if($1->pred->magicnumber == MAGIC_NUMBER_FALSE)
	{
		delete $1->pred;
		$1 -> pred = new FalsePredicate();
	}
	$$ = AtomicProposition($1); 
	$$ -> shape = $1 -> shape;
	$$ -> formula = $1 -> pred;
    }
| _NOT_ statepredicate
    { 
	switch($2 -> shape)
	{
	case AT_TEMP: 	$$ = Negation($2); 
			break;
	case AT_DL:
	case AT_FIR:
	case AT_COMP: 	$2 -> formula = $2 -> formula -> negate(); 
			$$ = $2; 
			break;
	case AT_TRUE: 	delete($2 -> formula);
			$2 -> formula = new FalsePredicate();
			$2 -> shape = AT_FALSE;
			$$ = $2;
			break;
	case AT_FALSE: 	delete($2 -> formula);
			$2 -> formula = new TruePredicate();
			$2 -> shape = AT_TRUE;
			$$ = $2;
			break;
	case AT_AND: 	$2 -> formula -> negate();
			$$ = $2;
			$$ -> shape = AT_OR;
			break;
	case AT_OR: 	$2 -> formula -> negate();
			$$ = $2;
			$$ -> shape = AT_AND;
			break;
	}
    }
| statepredicate _AND_ statepredicate
    { 
	if($1 -> shape == AT_TEMP || $3 -> shape == AT_TEMP)
	{
		$$ = Conjunction($1, $3); 
		$$ -> shape = AT_TEMP;
	}
	else if($1 -> shape == AT_FALSE || $3 -> shape == AT_FALSE)
	{
		delete $1 -> formula;
		delete $3 -> formula;
		$$ = $1;
		$$ -> formula = new FalsePredicate();
		$$ -> shape = AT_FALSE;
	}
	else if($1 -> shape == AT_TRUE)
	{
		$$ = $3;
		$$ -> shape = $3 -> shape;
	}
	else if($3 -> shape == AT_TRUE)
	{
		$$ = $1;
		$$ -> shape = $1 -> shape;
	} 
	else if(($1->shape == AT_AND) && (($3 -> shape == AT_OR) || ($3 -> shape == AT_FIR) || ($3 -> shape == AT_COMP) || ($3->shape == AT_DL)))
	{
		reinterpret_cast<AtomicBooleanPredicate *>($1 -> formula) -> addSub($3->formula);
		$$ = $1;
	}
	else if(($3->shape == AT_AND) && (($1 -> shape == AT_OR) || ($1 -> shape == AT_COMP) || ($1 -> shape == AT_DL) || ($1 -> shape == AT_FIR)))
	{
		reinterpret_cast<AtomicBooleanPredicate *>($3 -> formula) -> addSub($1->formula);
		$$ = $3;
	}
	else if(($1->shape == AT_AND) && ($3 -> shape == AT_AND))
	{
		reinterpret_cast<AtomicBooleanPredicate *>($3 -> formula)->merge(reinterpret_cast<AtomicBooleanPredicate *>($1->formula));
		$$ = $3;
	}
	else // both $1 and $3 are AT_FIR, AT_COMP, AT_DL or AT_OR
	{
		AtomicBooleanPredicate * result = new AtomicBooleanPredicate(true);
		result -> addSub($1->formula);
		result -> addSub($3->formula);
		$$ = $1;
		$$ -> formula = result;
		$$ -> shape = AT_AND;
	}
    }
| statepredicate _OR_ statepredicate
    { 
	if($1 -> shape == AT_TEMP || $3 -> shape == AT_TEMP)
	{
		$$ = Disjunction($1, $3); 
		$$ -> shape = AT_TEMP;
	}
	else if($1 -> shape == AT_TRUE || $3 -> shape == AT_TRUE)
	{
		delete $1 -> formula;
		delete $3 -> formula;
		$$ = $1;
		$$ -> formula = new TruePredicate();
		$$ -> shape = AT_TRUE;
	}
	else if($1 -> shape == AT_FALSE)
	{
		$$ = $3;
		$$ -> shape = $3 -> shape;
	}
	else if($3 -> shape == AT_FALSE)
	{
		$$ = $1;
		$$ -> shape = $1 -> shape;
	} 
	else if(($1->shape == AT_OR) && (($3 -> shape == AT_AND) || ($3 -> shape == AT_COMP) || ($3 -> shape == AT_FIR) || ($3->shape == AT_DL) ))
	{
		reinterpret_cast<AtomicBooleanPredicate *>($1 -> formula) -> addSub($3->formula);
		
		$$ = $1;
	}
	else if(($3->shape == AT_OR) && (($1 -> shape == AT_AND) || ($1 -> shape == AT_COMP) || ($1->shape == AT_FIR) || ($1->shape == AT_DL)))
	{
		reinterpret_cast<AtomicBooleanPredicate *>($3 -> formula) -> addSub($1->formula);
		$$ = $3;
	}
	else if(($1->shape == AT_OR) && ($3 -> shape == AT_OR))
	{
		reinterpret_cast<AtomicBooleanPredicate *>($3 -> formula)->merge(reinterpret_cast<AtomicBooleanPredicate *>($1->formula));
		$$ = $3;
	}
	else // both $1 and $3 are AT_COMP,AT_FIR,AT_DL or AT_OR
	{
		AtomicBooleanPredicate * result = new AtomicBooleanPredicate(false);
		result -> addSub($1->formula);
		result -> addSub($3->formula);
		$$ = $1;
		$$ -> formula = result;
		$$ -> shape = AT_OR;
	}
    }
| statepredicate _XOR_ statepredicate
    { 
	// translate into (p & -q) | (-p & q)

	if($1 -> shape == AT_TEMP || $3 -> shape == AT_TEMP)
	{
		$$ = ExclusiveDisjunction($1, $3); 
		$$ -> shape = AT_TEMP;
	}
	else if($1 -> shape == AT_TRUE)
	{
		$3 -> formula = $3 -> formula -> negate();
		$$ = $3;
		$$ -> shape = oppShape($3->shape);
	}
	else if($3 -> shape == AT_TRUE)
	{
		$1 -> formula = $1 -> formula -> negate();
		$$ = $1;
		$$ -> shape = oppShape($1->shape);
	}
	else if($3 -> shape == AT_FALSE)
	{
		$$ = $1;
	}
	else if($1 -> shape == AT_FALSE)
	{
		$$ = $3;
	}
	else if(($1 -> shape == AT_AND) && (($3 -> shape == AT_COMP)|| ($3->shape == AT_FIR) || ($3->shape == AT_DL)))
	{
		StatePredicate * b = $3 -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_*
		StatePredicate * c = $1->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_OR
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(c);
		d -> addSub($3->formula); // d = (-p & q)
		reinterpret_cast<AtomicBooleanPredicate *>($1 -> formula) ->addSub(b);     // $1 = (p & -q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub($1->formula);
		a -> addSub(d); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else if((($1 -> shape == AT_COMP)||($1->shape == AT_FIR) || ($1->shape == AT_DL)) && ($3 -> shape == AT_AND))
	{
		StatePredicate * b = $1 -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_ELEM
		StatePredicate * c = $3->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_OR
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(c);
		d -> addSub($1->formula); // d = (p & -q)
		reinterpret_cast<AtomicBooleanPredicate *>($3 -> formula) ->addSub(b);     // $3 = (-p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub($3->formula); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else if((($1 -> shape == AT_COMP)||($1->shape == AT_FIR) || ($1->shape == AT_DL)) && ($3 -> shape == AT_OR))
	{
		StatePredicate * b = $1 -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_ELEM
		StatePredicate * c = $3->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_AND
		reinterpret_cast<AtomicBooleanPredicate *>(c) -> addSub($1 -> formula); // c = p & -q
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(b);
		d -> addSub($3->formula); // d = (-p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub(c); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else if(($1 -> shape == AT_OR) && (($3 -> shape == AT_COMP)||($3->shape == AT_FIR) || ($3->shape == AT_DL)))
	{
		StatePredicate * b = $3 -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_ELEM
		StatePredicate * c = $1->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_AND
		reinterpret_cast<AtomicBooleanPredicate *>(c) -> addSub($3 -> formula); // c = -p & q
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(b);
		d -> addSub($1->formula); // d = (p & -q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub(c); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else if((($1 -> shape == AT_COMP)||($1->shape == AT_FIR) || ($1->shape == AT_DL)) && (($3 -> shape == AT_COMP)|| ($3->shape == AT_FIR)||($3->shape==AT_DL)))
	{
		StatePredicate * b = $3 -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_ELEM
		StatePredicate * c = $1->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_AND
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(b);
		d -> addSub($1->formula); // d = (p & -q)
		AtomicBooleanPredicate * e = new AtomicBooleanPredicate(true);
		e -> addSub(c);
		e -> addSub($3->formula); // d = (p & -q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub(e); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else if(($1 -> shape == AT_AND) && ($3 -> shape == AT_AND))
	{
		StatePredicate * b = $3 -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_OR
		StatePredicate * c = $1->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_OR
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(b);
		d -> addSub($1->formula); // d = (p & -q)
		AtomicBooleanPredicate * e = new AtomicBooleanPredicate(true);
		e -> addSub(c);
		e -> addSub($3->formula); // d = (p & -q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub(e); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else if(($1 -> shape == AT_AND) && ($3 -> shape == AT_OR))
	{
		StatePredicate * b = $3 -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_AND
		StatePredicate * c = $1->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_OR
		reinterpret_cast<AtomicBooleanPredicate *>(b) -> addSub($1->formula); // p & -q
		AtomicBooleanPredicate * e = new AtomicBooleanPredicate(true);
		e -> addSub(c);
		e -> addSub($3->formula); // e = (-p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(b);
		a -> addSub(e); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else if(($1 -> shape == AT_OR) && ($3 -> shape == AT_AND))
	{
		StatePredicate * b = $1 -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_AND
		StatePredicate * c = $3->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_OR
		reinterpret_cast<AtomicBooleanPredicate *>(b) -> addSub($3->formula); // -p & q
		AtomicBooleanPredicate * e = new AtomicBooleanPredicate(true);
		e -> addSub(c);
		e -> addSub($1->formula); // e = (p & -q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(b);
		a -> addSub(e); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else // (($1 -> shape == AT_OR) && ($3 -> shape == AT_OR))
	{
		StatePredicate * b = $1 -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_AND
		StatePredicate * c = $3->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_AND
		reinterpret_cast<AtomicBooleanPredicate *>(b) -> addSub($3->formula); // -p & q
		reinterpret_cast<AtomicBooleanPredicate *>(c) -> addSub($1->formula); // p & -q
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(b);
		a -> addSub(c); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
    }
| statepredicate _implies_ statepredicate
    { 
	if($1 -> shape == AT_TEMP || $3 -> shape == AT_TEMP)
	{
		$$ = Implication($1, $3); 
		$$ -> shape = AT_TEMP;
	}
	else if($1 -> shape == AT_FALSE || $3 -> shape == AT_TRUE)
	{
		delete $1 -> formula;
		delete $3 -> formula;
		$$ = $1;
		$$ -> formula = new TruePredicate();
		$$ -> shape = AT_TRUE;
	}
	else if($1 -> shape == AT_TRUE)
	{
		$$ = $3;
		$$ -> shape = $3 -> shape;
	}
	else if($3 -> shape == AT_FALSE)
	{
		$1->formula = $1 -> formula -> negate();
		$$ = $1;
		$$ -> shape = oppShape($1 -> shape);
	} 
	else if(($1->shape == AT_AND) && (($3 -> shape == AT_AND) || ($3 -> shape == AT_COMP)|| ($3->shape == AT_FIR) || ($3->shape == AT_DL)))
	{
		 $1 -> formula = $1 -> formula ->negate();
		reinterpret_cast<AtomicBooleanPredicate *>($1 -> formula) -> addSub($3->formula);
		$$ = $1;
		$$ -> shape = AT_OR;
	}
	else if(($3->shape == AT_OR) && (($1 -> shape == AT_AND) || ($1 -> shape == AT_COMP)||($1->shape == AT_FIR) || ($1->shape == AT_DL)))
	{
		$1 -> formula = $1-> formula -> negate();
		reinterpret_cast<AtomicBooleanPredicate *>($3 -> formula) -> addSub($1->formula);
		$$ = $3;
	}
	else if(($1->shape == AT_AND) && ($3 -> shape == AT_OR))
	{
		$1 -> formula = $1 -> formula -> negate();
		reinterpret_cast<AtomicBooleanPredicate *>($3 -> formula)->merge(reinterpret_cast<AtomicBooleanPredicate *>($1->formula));
		$$ = $3;
	}
	else // both $1 and $3 are AT_* or AT_OR
	{
		$1 -> formula = $1 -> formula -> negate();
		AtomicBooleanPredicate * result = new AtomicBooleanPredicate(false);
		result -> addSub($1->formula);
		result -> addSub($3->formula);
		$$ = $1;
		$$ -> formula = result;
		$$ -> shape = AT_OR;
	}
    }
| statepredicate _iff_ statepredicate
    { 
	// translate into (p & q) | (-p & -q)

	if($1 -> shape == AT_TEMP || $3 -> shape == AT_TEMP)
	{
		$$ = Equivalence($1, $3); 
		$$ -> shape = AT_TEMP;
	}
	else if($1 -> shape == AT_FALSE)
	{
		$3 -> formula = $3 -> formula -> negate();
		$$ = $3;
		$$ -> shape = oppShape($3->shape);
	}
	else if($3 -> shape == AT_FALSE)
	{
		$1 -> formula = $1 -> formula -> negate();
		$$ = $1;
		$$ -> shape = oppShape($1->shape);
	}
	else if($3 -> shape == AT_TRUE)
	{
		$$ = $1;
	}
	else if($1 -> shape == AT_TRUE)
	{
		$$ = $3;
	}
	else if(($1 -> shape == AT_AND) && (($3 -> shape == AT_COMP)||($3->shape == AT_FIR) || ($3->shape == AT_DL)))
	{
		StatePredicate * b = $3 -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_ELEM
		StatePredicate * c = $1->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_OR
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(c);
		d -> addSub(b); // d = (-p & -q)
		reinterpret_cast<AtomicBooleanPredicate *>($1 -> formula) ->addSub($3->formula);     // $1 = (p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub($1->formula);
		a -> addSub(d); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else if((($1 -> shape == AT_COMP)|| ($1->shape == AT_FIR) || ($1->shape == AT_DL)) && ($3 -> shape == AT_AND))
	{
		StatePredicate * b = $1 -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_ELEM
		StatePredicate * c = $3->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_OR
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(c);
		d -> addSub(b); // d = (-p & -q)
		reinterpret_cast<AtomicBooleanPredicate *>($3 -> formula) ->addSub($1->formula);     // $3 = (p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub($3->formula); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else if((($1 -> shape == AT_COMP)||($1->shape == AT_FIR) || ($1->shape == AT_DL)) && ($3 -> shape == AT_OR))
	{
		StatePredicate * b = $1 -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_ELEM
		StatePredicate * c = $3->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_AND
		reinterpret_cast<AtomicBooleanPredicate *>(c) -> addSub(b); // c = -p & -q
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub($1->formula);
		d -> addSub($3->formula); // d = (p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub(c); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else if(($1 -> shape == AT_OR) && (($3 -> shape == AT_COMP)||($3->shape==AT_FIR) || ($3->shape == AT_DL)))
	{
		StatePredicate * b = $3 -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_ELEM
		StatePredicate * c = $1->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_AND
		reinterpret_cast<AtomicBooleanPredicate *>(c) -> addSub(b); // c = -p & -q
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub($3->formula);
		d -> addSub($1->formula); // d = (p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub(c); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else if((($1 -> shape == AT_COMP)|| ($1->shape == AT_FIR) || ($1->shape == AT_DL)) && (($3 -> shape == AT_COMP)||($3->shape == AT_FIR)||($3->shape == AT_DL)))
	{
		StatePredicate * b = $3 -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_ELEM
		StatePredicate * c = $1->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_AND
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(b);
		d -> addSub(c); // d = (-p & -q)
		AtomicBooleanPredicate * e = new AtomicBooleanPredicate(true);
		e -> addSub($1->formula);
		e -> addSub($3->formula); // e = (p & q)
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub(e); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else if(($1 -> shape == AT_AND) && ($3 -> shape == AT_AND))
	{
		StatePredicate * b = $3 -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_OR
		StatePredicate * c = $1->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_OR
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub(b);
		d -> addSub(c); // d = (-p & -q)
		reinterpret_cast<AtomicBooleanPredicate *>($1->formula)->merge(reinterpret_cast<AtomicBooleanPredicate *>($3->formula)); //$1 = p&q
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(d);
		a -> addSub($1->formula); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else if(($1 -> shape == AT_AND) && ($3 -> shape == AT_OR))
	{
		StatePredicate * b = $3 -> formula -> copy(NULL);
		b = b -> negate();  //  -q, AT_AND
		StatePredicate * c = $1->formula -> copy(NULL);
		c = c -> negate(); // -p, AT_OR
		reinterpret_cast<AtomicBooleanPredicate *>(b) -> addSub(c); // -p & -q
		reinterpret_cast<AtomicBooleanPredicate *>($1->formula)->addSub($3->formula); // $1 = p&q
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(b);
		a -> addSub($1->formula); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else if(($1 -> shape == AT_OR) && ($3 -> shape == AT_AND))
	{
		StatePredicate * b = $1 -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_AND
		StatePredicate * c = $3->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_OR
		reinterpret_cast<AtomicBooleanPredicate *>(b) -> addSub(c); // -p & -q
		reinterpret_cast<AtomicBooleanPredicate *>($3->formula)->addSub($1->formula); //$3 = p&q
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(b);
		a -> addSub($3->formula); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
	else // (($1 -> shape == AT_OR) && ($3 -> shape == AT_OR))
	{
		StatePredicate * b = $1 -> formula -> copy(NULL);
		b = b -> negate();  //  -p, AT_AND
		StatePredicate * c = $3->formula -> copy(NULL);
		c = c -> negate(); // -q, AT_AND
		reinterpret_cast<AtomicBooleanPredicate *>(b) -> merge(reinterpret_cast<AtomicBooleanPredicate *>(c)); // -p & -q
		AtomicBooleanPredicate * d = new AtomicBooleanPredicate(true);
		d -> addSub($1->formula);
		d -> addSub($3->formula); // d = p&q
		AtomicBooleanPredicate * a = new AtomicBooleanPredicate(false);
		a -> addSub(b);
		a -> addSub(d); // a = result
		$$ = $1;
		$$ -> formula = a;
		$$ -> shape = AT_OR;
	}
    }
| _ALLPATH_ statepredicate
    { $$ = AllPath($2); }
| _EXPATH_ statepredicate
    { $$ = ExPath($2); }
| _ALWAYS_ statepredicate
    { $$ = Always($2); }
| _EVENTUALLY_ statepredicate
    { $$ = Eventually($2); }
| _CTLOPERATOR_ statepredicate
    {
        kc::tStatePredicate result = $2;
        std::string op($1->name);
        for (int i = op.size()-1; i >= 0; i--)
        {
            if (op[i] == 'A') result = AllPath(result);
            if (op[i] == 'E') result = ExPath(result);
            if (op[i] == 'F') result = Eventually(result);
            if (op[i] == 'G') result = Always(result);
            if (op[i] == 'X') result = NextState(result);
            if (op[i] == 'U') yyerrors($1->name, @1, "operator 'U' is not allowed here");
            if (op[i] == 'R') yyerrors($1->name, @1, "operator 'R' is not allowed here");
        }
        $$ = result;
    }
| _leftparenthesis_ statepredicate _UNTIL_ statepredicate _rightparenthesis_
    { $$ = Until($2, $4); }
| _leftparenthesis_ statepredicate _RELEASE_ statepredicate _rightparenthesis_
    { $$ = Release($2, $4); }
| _leftparenthesis_ statepredicate _CTLOPERATOR_ statepredicate _rightparenthesis_
    {
        std::string op($3->name);
        if (op == "R") {
            $$ = Release($2, $4);
        } else if (op == "U") {
            $$ = Until($2, $4);
        } else {
            yyerrors($3->name, @3, "operator '%s' is not allowed here", $3->name);
        }
    }
| _NEXTSTATE_ statepredicate
    { $$ = NextState($2); }
| _REACHABLE_ statepredicate
    { $$ = ExPath(Eventually($2)); }
| _INVARIANT_ statepredicate
    { $$ = AllPath(Always($2)); }
| _IMPOSSIBLE_ statepredicate
    { 
	$$ = AllPath(Always(Negation($2))); 
    }
;

atomic_proposition:
  term _equals_ term
    { 
	$3 -> ttt -> multiply(-1);
	$1 -> ttt -> append($3 -> ttt);
	Term * T = $1 -> ttt -> copy();
	T -> multiply(-1);
	AtomicBooleanPredicate * result = new AtomicBooleanPredicate(true);
	result -> addSub(new AtomicStatePredicate($1->ttt));	
	result -> addSub(new AtomicStatePredicate(T));	
	$$ = Elementary();
	$$ -> shape = AT_AND;
	$$ -> pred = result;
    }
| term _notequal_ term
    { 
	$3 -> ttt -> multiply(-1);
	$1 -> ttt -> append($3 -> ttt);
	Term * T = $1 -> ttt -> copy();
	T -> multiply(-1);
	AtomicBooleanPredicate * result = new AtomicBooleanPredicate(true);
	result -> addSub(new AtomicStatePredicate($1->ttt));	
	result -> addSub(new AtomicStatePredicate(T));	
	result = (AtomicBooleanPredicate *) result -> negate();
	$$ = Elementary();
	$$ -> shape = AT_OR;
	$$ -> pred = result;
    }
| term _greaterthan_ term
    { 
	$1 -> ttt -> multiply(-1);
	$1 -> ttt -> append($3 -> ttt);
	Term * T = new Term();
	T -> place = Net::Card[PL];
	T -> mult = 1;
	$1 -> ttt -> append(T);
	AtomicStatePredicate * result = new AtomicStatePredicate($1->ttt);
	$$ = Elementary();
	$$ -> shape = AT_COMP;
	$$ -> pred = result;
	
    }
| term _greaterorequal_ term
    { 
	$1 -> ttt -> multiply(-1);
	$1 -> ttt -> append($3 -> ttt);
	AtomicStatePredicate * result = new AtomicStatePredicate($1->ttt);
	$$ = Elementary();
	$$ -> shape = AT_COMP;
	$$ -> pred = result;
    }
| term _lessthan_ term
    { 
	$3 -> ttt -> multiply(-1);
	$1 -> ttt -> append($3 -> ttt);
	Term * T = new Term();
	T -> place = Net::Card[PL];
	T -> mult = 1;
	$1 -> ttt -> append(T);
	AtomicStatePredicate * result = new AtomicStatePredicate($1->ttt);
	$$ = Elementary();
	$$ -> shape = AT_COMP;
	$$ -> pred = result;
    }
| term _lessorequal_ term
    { 
	$3 -> ttt -> multiply(-1);
	$1 -> ttt -> append($3 -> ttt);
	AtomicStatePredicate * result = new AtomicStatePredicate($1->ttt);
	$$ = Elementary();
	$$ -> shape = AT_COMP;
	$$ -> pred = result;
    }
| _TRUE_
    { 
	TruePredicate * result = new TruePredicate();
	$$ = Elementary();
	$$ -> shape = AT_TRUE;
	$$ -> pred = result;
    }
| _FALSE_
    { 
	FalsePredicate * result = new FalsePredicate();
	$$ = Elementary();
	$$ -> shape = AT_FALSE;
	$$ -> pred = result;
    }
| _FIREABLE_ _leftparenthesis_ identifier _rightparenthesis_
    {
        Symbol *t = symbolTables->TransitionTable->lookup($3->name);
        if (UNLIKELY(t == NULL))
        {
            yyerrors($3->name, @3, "transition '%s' unknown", $3->name);
        }
	FireablePredicate * result = new FireablePredicate(t->getIndex(),true);
        target_transition.insert(t->getIndex());
	$$ = Elementary();
	$$ -> shape = AT_FIR;
	$$ -> pred = result;
    }
| _INITIAL_
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
	$$ = Elementary(); 
	$$ -> shape = AT_AND;
	$$ -> pred = result;
    }
| _DEADLOCK_
    { 
	DeadlockPredicate * result = new DeadlockPredicate(true);
	$$ = Elementary(); 
	$$ -> shape = AT_DL;
	$$ -> pred = result;
    }
;

computeboundformula:
	_MAX_ _leftparenthesis_ term _rightparenthesis_ 
	{
		$$ = CompBound();
		$$ -> formula = new AtomicStatePredicate($3->ttt);
	}

term:
  _leftparenthesis_ term _rightparenthesis_
    { $$ = $2; }
| identifier
    {
        Symbol *p = symbolTables->PlaceTable->lookup($1->name);
        if (UNLIKELY(p == NULL))
        {
            yyerrors($1->name, @1, "place '%s' unknown", $1->name);
        }
	$$ = Complex();
	$$ -> ttt = new Term();
	$$ -> ttt -> place = p -> getIndex();
	$$ -> ttt -> mult = 1;
        target_place.insert(p->getIndex());
#ifdef RERS
	rers_place[p->getIndex()] = true;
#endif
    }
| NUMBER
    { 
	$$ = Complex(); 
	$$ -> ttt = new Term();
	$$ -> ttt -> place = Net::Card[PL];
	$$ -> ttt -> mult = $1 -> value;
    }
| term _plus_ term
    { 
	$1 -> ttt -> append($3 -> ttt);
	$$ = $1; 
    }
| term _minus_ term
    { 
	$3 -> ttt -> multiply(-1);
	$1 -> ttt -> append($3 -> ttt);
	$$ = $1; 
    }
| NUMBER _times_ term
    { 
	$3 -> ttt->multiply($1->value);
	$$ = $3; 
    }
| _omega_
    { 
      $$ = Complex();
      $$ -> ttt = new Term();
      $$ -> ttt -> place = Net::Card[PL];
      $$ -> ttt -> mult = OMEGA;
      if (RT::args.search_arg != search_arg_covergraph)
      {
          RT::rep->message("%s: omega markings used without %s",
              RT::rep->markup(MARKUP_WARNING, "warning").str(),
              RT::rep->markup(MARKUP_PARAMETER, "--search=cover").str());
      }
    }
;

identifier:
  IDENTIFIER     { $$ = $1; }
| _CTLOPERATOR_  { $$ = $1; }
;

%%

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

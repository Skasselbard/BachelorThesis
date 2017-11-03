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

%{ KC_UNPARSE
// LCOV_EXCL_START
/*!
\brief printer-function for Kimiwtu's output on stdout

To use an unparser in Kimwitu++, a function with this signature needs to be
passed to the unparse() function.

\param[in] s  string to print

\note This function is not "found" by the code coverage detection and is hence
hidden from it.
*/
void myprinter(const char *s, kc::uview)
{
    printf("%s", s);
}
// LCOV_EXCL_STOP

/// result string for stringprinter
std::string unparsed;

// LCOV_EXCL_START
/*!
\brief printer-function for Kimiwtu's output to a string object

To use an unparser in Kimwitu++, a function with this signature needs to be
passed to the unparse() function.

\param[in] s  string to add

\note This function is not "found" by the code coverage detection and is hence
hidden from it.
\post Output is written to string unparsed which can be accessed wherever an
unparser can be used. You need to clear the string after use to avoid keeping
old data.
*/
void stringprinter(const char *s, kc::uview)
{
    unparsed += s;
}
// LCOV_EXCL_STOP

bool *place_in_formula = NULL;
unsigned int places_mentioned = 0;
unsigned int unique_places_mentioned = 0;
%}

%{ KC_UNPARSE_HEADER
#include <CoverGraph/CoverGraph.h>
#include <Formula/StatePredicate/DeadlockPredicate.h>
#include <Formula/StatePredicate/FireablePredicate.h>
#include <Net/Net.h>
#include <Net/Transition.h>
#include <string>

extern std::string unparsed;

void myprinter(const char *s, kc::uview v);
void stringprinter(const char *s, kc::uview v);
%}



//////////////////////////////////////////////////////////////////////////
%uview out;

StatePredicateFormula(x) -> [out: x];
Compound(x,y) -> [out:  x " : " y ];
c=CompBound() -> [out:  "MAX(" {kc_printer(((AtomicStatePredicate *)(c->formula))->toCompString(),kc_current_view);} ")"];

a=AtomicProposition(x=Elementary()) -> [out:  {kc_printer(x->pred->toString(),kc_current_view);}];
a=AtomicProposition(NNegation(x=Elementary())) -> [out: {StatePredicate * t=x->pred->copy(NULL)->negate();kc_printer(t->toString(),kc_current_view);delete t;}];
Negation(x) -> [out: "NOT(" x ")"];
Conjunction(x, y) -> [out:  "(" x " AND " y ")"];
Disjunction(x, y) -> [out:   "(" x " OR " y ")" ];
ExclusiveDisjunction(x, y) -> [out: "(" x " XOR " y ")"];
Implication(x, y) -> [out: "(" x " -> " y ")"];
Equivalence(x, y) -> [out: "(" x " <-> " y ")"];
AllPath(x) -> [out: "A (" x ")"];
ExPath(x)  -> [out: "E (" x ")"];
Always(x)     -> [out: "G ("  x  ")"];
Eventually(x) -> [out: "F (" x ")"];
NextState(x) -> [out: "X (" x ")"];
Until(phi,psi) -> [out: "(" phi " U " psi ")"];
Release(phi,psi) -> [out: "(" phi " R " psi ")"];
EU(x,y) -> [out: "E(" x " U " y ")"];
AR(x,y) -> [out: "A(" x " R " y ")"];
ER(x,y) -> [out: "E(" x " R " y ")"];
AU(x,y) -> [out: "A(" x " U " y ")"];
AF(x)   -> [out: "AF(" x ")"];
AG(x)   -> [out: "AG("  x  ")"];
AX(x)   -> [out: "AX(" x ")"];
EX(x)   -> [out: "EX(" x ")"];
EF(x)   -> [out: "EF(" x ")"];
EG(x)   -> [out: "EG(" x ")"];

ConjunctionList(l) -> [out: "(" l ")"];
ConstConjunction_list(h, NiltConjunction_list()) -> [out: h];
ConstConjunction_list(h, l) -> [out: h " AND " l];

DisjunctionList(l) -> [out: "(" l ")"];
ConstDisjunction_list(h, NiltDisjunction_list()) -> [out: h];
ConstDisjunction_list(h, l) -> [out: h " OR " l];


True() -> [out: "TRUE"];
False() -> [out: "FALSE"];
Deadlock() -> [out: "DEADLOCK"];
NoDeadlock() -> [out: "NOT DEADLOCK"];

BuechiAutomaton(trans,accept) -> [out: trans "\n" accept];
BuechiNull() -> [out:];

EmptyBuechiRules() -> [out:];
BuechiRule(i,t) -> [out: "STATE" i t];
ExpandedBuechiRule(from, formula, to) -> [out: from "(" formula ") -> " to "\n"];
BuechiRules(a,b) -> [out: a "&" b];

EmptyTransitionRules() -> [out:];
TransitionRule(formu,i) -> [out: formu "->" i];
TransitionRules(a,b) -> [out: a "&" b];

EmptyAcceptingSet() -> [out:];
AcceptingState(*) -> [out:];
AcceptingSet(*,*) -> [out:];



//////////////////////////////////////////////////////////////////////////
%uview count;

StatePredicateFormula(x) -> [count: {x->fs = fs;} x {fs = x->fs;}];
Compound(x,y) -> [count: {x->fs=fs;} x {y->fs=x->fs;} y {fs=y->fs;}];
c=CompBound() -> [count: {fs=c->formula->count(fs);fs->comp--;}];

a=AtomicProposition(x=Elementary()) -> [count: {fs=x->pred->count(fs);}];
a=AtomicProposition(NNegation(x=Elementary())) -> [count: {fs=x->pred->count(fs);fs->aneg++;}];
AtomicProposition(x) -> [count:  ];
Negation(x) -> [count: {x->fs=fs;} x {fs=x->fs;fs->tneg++;}];
Conjunction(x, y) -> [count:  {x->fs=fs;} x {y->fs=x->fs;} y {fs=y->fs;fs->tconj++;}];
Disjunction(x, y) -> [count:  {x->fs=fs;} x {y->fs=x->fs;} y {fs=y->fs;fs->tdisj++;}];
AllPath(x) -> [count: {x->fs=fs;} x {fs=x->fs;fs->A++;}];
ExPath(x)  -> [count: {x->fs=fs;} x {fs=x->fs;fs->E++;}];
Always(x)     -> [count: {x->fs=fs;}  x  {fs=x->fs;fs->G++;}];
Eventually(x) -> [count: {x->fs=fs;} x {fs=x->fs;fs->F++;}];
NextState(x) -> [count: {x->fs=fs;} x {fs=x->fs;fs->X++;}];
Until(phi,psi) -> [count: {phi->fs=fs;} phi {psi->fs=phi->fs;} psi {fs=psi->fs;fs->U++;}];
Release(phi,psi) -> [count: {phi->fs=fs;} phi {psi->fs=phi->fs;} psi {fs=psi->fs;fs->R++;}];
EU(x,y) -> [count: {x->fs=fs;} x {y->fs=x->fs;} y {fs=y->fs;fs->E++;fs->U++;}];
AR(x,y) -> [count: {x->fs=fs;} x {y->fs=x->fs;} y {fs=y->fs;fs->A++;fs->R++;}];
ER(x,y) -> [count: {x->fs=fs;} x {y->fs=x->fs;} y {fs=y->fs;fs->E++;fs->R++;}];
AU(x,y) -> [count: {x->fs=fs;} x {y->fs=x->fs;} y {fs=y->fs;fs->A++;fs->U++;}];
AF(x)   -> [count: {x->fs=fs;} x {fs=x->fs;fs->A++;fs->F++;}];
AG(x)   -> [count: {x->fs=fs;} x {fs=x->fs;fs->A++;fs->G++;}];
AX(x)   -> [count: {x->fs=fs;} x {fs=x->fs;fs->A++;fs->X++;}];
EX(x)   -> [count: {x->fs=fs;} x {fs=x->fs;fs->E++;fs->X++;}];
EF(x)   -> [count: {x->fs=fs;} x {fs=x->fs;fs->E++;fs->F++;}];
EG(x)   -> [count: {x->fs=fs;} x {fs=x->fs;fs->E++;fs->G++;}];

ConjunctionList(l) -> [count: {l->fs=fs;} l {fs=l->fs;fs->tconj++;}];
ConstConjunction_list(h, NiltConjunction_list()) -> [count: {h->fs=fs;} h {fs=h->fs;}];
ConstConjunction_list(h, l) -> [count: {h->fs=fs;} h {l->fs=h->fs;} l {fs =l->fs;}];

DisjunctionList(l) -> [count: {l->fs=fs;} l {fs=l->fs;fs->tdisj++;}];
ConstDisjunction_list(h, NiltDisjunction_list()) -> [count: {h->fs=fs;} h {fs=h->fs;}];
ConstDisjunction_list(h, l) -> [count: {h->fs=fs;} h {l->fs=h->fs;} l {fs=l->fs;}];


True() -> [count: {fs->taut++;}];
False() -> [count: {fs->cont++;}];

//////////////////////////////////////////////////////////////////////////
%uview elem;

StatePredicateFormula(x) -> [elem: x];
Compound(x,y) -> [elem:  x  y ];
c=CompBound() -> [elem: ];

a=AtomicProposition(x=Elementary()) -> [elem:  {x->pred = a -> formula;}];
a=AtomicProposition(NNegation(x=Elementary())) -> [elem:  {x->pred=a->formula;}];
Negation(x) -> [elem:  x];
Conjunction(x, y) -> [elem: x y];
Disjunction(x, y) -> [elem: x y];
ExclusiveDisjunction(x, y) -> [elem: x y];
Implication(x, y) -> [elem: x  y];
Equivalence(x, y) -> [elem: x y];
AllPath(x) -> [elem:  x ];
ExPath(x)  -> [elem:  x];
Always(x)     -> [elem:  x ];
Eventually(x) -> [elem:  x ];
NextState(x) -> [elem: x];
Until(phi,psi) -> [elem: phi  psi];
Release(phi,psi) -> [elem: phi  psi ];
EU(x,y) -> [elem: x  y ];
AR(x,y) -> [elem:  x y ];
ER(x,y) -> [elem: x y ];
AU(x,y) -> [elem: x y ];
AF(x)   -> [elem: x ];
AG(x)   -> [elem: x];
AX(x)   -> [elem: x];
EX(x)   -> [elem: x];
EF(x)   -> [elem: x];
EG(x)   -> [elem:  x];

ConjunctionList(l) -> [elem: l];
ConstConjunction_list(h, NiltConjunction_list()) -> [elem: h];
ConstConjunction_list(h, l) -> [elem: h  l];

DisjunctionList(l) -> [elem: l ];
ConstDisjunction_list(h, NiltDisjunction_list()) -> [elem: h];
ConstDisjunction_list(h, l) -> [elem: h l];

//////////////////////////////////////////////////////////////////////////
%uview temporal;

// assumption: ExclusiveDisjunction, Implication, and Equivalence are gone.

f=Compound(x,y) -> [temporal: x y
    {
	f -> type = FORMULA_COMPOUND; 
	f -> cannotcompute = x -> cannotcompute || y -> cannotcompute;
    }];

f=CompBound() -> [temporal: 
    {
	f -> type = FORMULA_BOUND; 
	f -> cannotcompute = false;
    }];


f=StatePredicateFormula(x) -> [temporal: x 
    {
	f -> type = x -> type; 
        f -> cannotcompute = x -> cannotcompute || 
                (! x -> validLTLFormula) || (! x -> validCTLStateFormula);
    }];

f=ExPath(g=Eventually(h=AtomicProposition(Deadlock()))) -> [temporal: 
    { 
	f -> type = FORMULA_DEADLOCK;
	f -> cannotcompute = false;
	f -> validCTLStateFormula = true;
	f -> validCTLPathFormula = false;
	f -> validLTLFormula = false;
        f -> containsTemporal = true;
        g -> containsTemporal = true;
        h -> containsTemporal = false;
    }];

f=AllPath(g=Always(h=AtomicProposition(NoDeadlock())))  -> [temporal: 
    { 
	f -> type = FORMULA_NODEADLOCK;
	f -> cannotcompute = false;
	f -> validCTLStateFormula = true;
	f -> validCTLPathFormula = false;
	f -> validLTLFormula = false;
        f -> containsTemporal = true;
        g -> containsTemporal = true;
        h -> containsTemporal = false;
    }];

f=AllPath(g=Always(h=ExPath(i=Eventually(j=AllPath(k=Always(y)))))) -> [temporal: y
    { 
	if(not y->containsTemporal)
	{
		f -> type = FORMULA_AGEFAG;
		f -> cannotcompute = false;
		f -> validCTLStateFormula = true;
	}
	else
	{
		f -> cannotcompute = (y->cannotcompute || !y -> validCTLStateFormula);
		f -> validCTLStateFormula = y -> validCTLStateFormula;
		if( ! f -> cannotcompute)
		{
			f -> type = FORMULA_CTL; 
		}
	}
	f -> validCTLPathFormula = false;
	f -> validLTLFormula = false;
        f -> containsTemporal = true;
        g -> containsTemporal = true;
        h -> containsTemporal = true;
        i -> containsTemporal = true;
        j -> containsTemporal = true;
        k -> containsTemporal = true;
    }];

f=ExPath(g=Eventually(h=AllPath(i=Always(j=ExPath(k=Eventually(y)))))) -> [temporal: y
    { 
	if(not y->containsTemporal)
	{
		f -> type = FORMULA_EGAGEF;
		f -> cannotcompute = false;
		f -> validCTLStateFormula = true;
	}
	else
	{
		f -> cannotcompute = (y->cannotcompute || !y -> validCTLStateFormula);
		f -> validCTLStateFormula = y -> validCTLStateFormula;
		if(!f -> cannotcompute)
		{
			f -> type = FORMULA_CTL; 
		}
	}
	f -> validCTLPathFormula = false;
	f -> validLTLFormula = false;
        f -> containsTemporal = true;
        g -> containsTemporal = true;
        h -> containsTemporal = true;
        i -> containsTemporal = true;
        j -> containsTemporal = true;
        k -> containsTemporal = true;
    }];

f=ExPath(g=Eventually(h=AllPath(i=Always(y)))) ->[temporal: y
    { 
	if(not y->containsTemporal)
	{
		f -> type = FORMULA_EFAG;
		f -> cannotcompute = false;
		f -> validCTLStateFormula = true;
	}
	else
	{
		f -> cannotcompute = (y->cannotcompute || ! y -> validCTLStateFormula);
		f -> validCTLStateFormula = y -> validCTLStateFormula;
		if(!f -> cannotcompute)
		{
			f -> type = FORMULA_CTL; 
		}
	}
	f -> validCTLPathFormula = false;
	f -> validLTLFormula = false;
        f -> containsTemporal = true;
        g -> containsTemporal = true;
        h -> containsTemporal = true;
        i -> containsTemporal = true;
    }];

f=AllPath(g=Always(h=ExPath(i=Eventually(y)))) -> [temporal: y 
    { 
	if(not y->containsTemporal)
	{
		f -> type = FORMULA_LIVENESS;
		f -> cannotcompute = false;
		f -> validCTLStateFormula = true;
	}
	else
	{
		f -> cannotcompute = (y->cannotcompute || !y -> validCTLStateFormula);
		f -> validCTLStateFormula = y -> validCTLStateFormula;
		if(! f -> cannotcompute)
		{
			f -> type = FORMULA_CTL; 
		}
	}
	f -> validCTLPathFormula = false;
	f -> validLTLFormula = false;
        f -> containsTemporal = true;
        g -> containsTemporal = true;
        h -> containsTemporal = true;
        i -> containsTemporal = true;
    }];

f=ExPath(g=Eventually(y)) -> [temporal: y 
    { 
	if(not y->containsTemporal)
	{
		f -> type = FORMULA_REACHABLE;
		f -> cannotcompute = false;
		f -> validCTLStateFormula = true;
	}
	else
	{
		f -> cannotcompute = (y->cannotcompute || !y -> validCTLStateFormula);
		f -> validCTLStateFormula = y -> validCTLStateFormula;
		if(!f -> cannotcompute)
		{
			f -> type = FORMULA_CTL; 
		}
	}
	f -> validCTLPathFormula = false;
	f -> validLTLFormula = false;
        f -> containsTemporal = true;
        g -> containsTemporal = true;
    }];

f=AllPath(g=Always(y)) -> [temporal: y 
    { 
	if(not y->containsTemporal)
	{
		f -> type = FORMULA_INVARIANT;
		f -> cannotcompute = false;
		f -> validCTLStateFormula = true;
                f -> validLTLFormula = y -> validLTLFormula;
	}
	else
	{   
		f -> cannotcompute = (y->cannotcompute || !y -> validCTLStateFormula);
		f -> validCTLStateFormula = y -> validCTLStateFormula;
		if(! f -> cannotcompute)
		{
			if(y->validLTLFormula)
			{
				f->type = FORMULA_BOTH;
			}
			else
			{
				f -> type = FORMULA_CTL; 
			}
		}
                else
                {
                    f -> validLTLFormula = false;
                    if (y -> validLTLFormula)
                    {
                        f -> type = FORMULA_LTL;
                        f -> cannotcompute = false;
                    }
                }
	}
	f -> validCTLPathFormula = false;
        f -> containsTemporal = true;
        g -> containsTemporal = true;
    }];

f=Always(g=Eventually(y)) -> [temporal: y
    { 
	if(not y->containsTemporal)
	{
		f -> type = FORMULA_FAIRNESS;
		f -> cannotcompute = false;
		f -> validLTLFormula = true;
	}
	else
	{
		f -> cannotcompute = (y->cannotcompute || ! y -> validLTLFormula);
		f -> validLTLFormula = y -> validLTLFormula;
		if(!f -> cannotcompute)
		{
			f -> type = FORMULA_LTL; 
		}
	}
	f -> validCTLPathFormula = false;
	f -> validCTLStateFormula = false;
        f -> containsTemporal = true;
        g -> containsTemporal = true;
    }];

f=Eventually(g=Always(y)) -> [temporal: y
    { 
	if(not y->containsTemporal)
	{
		f -> type = FORMULA_STABILIZATION;
		f -> cannotcompute = false;
		f -> validLTLFormula = true;
	}
	else
	{
		f -> cannotcompute = (y->cannotcompute || ! y -> validLTLFormula);
		f -> validLTLFormula = y -> validLTLFormula;
		if(!f -> cannotcompute)
		{
			f -> type = FORMULA_LTL; 
		}
	}
	f -> validCTLPathFormula = false;
	f -> validCTLStateFormula = false;
        f -> containsTemporal = true;
        g -> containsTemporal = true;
    }];

f=Eventually(y) -> [temporal: y
    { 
	if(not y->containsTemporal)
	{
		f -> type = FORMULA_EVENTUALLY;
		f -> cannotcompute = false;
		f -> validLTLFormula = true;
	}
	else
	{
		f -> cannotcompute = (y->cannotcompute || ((!y -> validLTLFormula) && !(y -> validCTLStateFormula)));
		f -> validLTLFormula = y -> validLTLFormula;
		if(!f -> cannotcompute)
		{
			f -> type = FORMULA_LTL; 
		}
	}
	f -> validCTLPathFormula = y -> validCTLStateFormula;
	f -> validCTLStateFormula = false;
        f -> containsTemporal = true;
    }];

f=AtomicProposition(*) -> [temporal: 
    {
        f->type = FORMULA_INITIAL;
	f->cannotcompute=false;
    	f->containsTemporal = false;
    	f->validCTLPathFormula = false;
    	f->validCTLStateFormula = true;
    	f->validLTLFormula = true;
    }];

f=Negation(x) -> [temporal: x 
    {
	if(not x->containsTemporal)
	{
        	f->type = FORMULA_INITIAL;
	}
	else
	{
        	f->type = FORMULA_BOOLEAN;
	}
	f->cannotcompute=x->cannotcompute;
	if(f->cannotcompute)
	{
		f->type = FORMULA_MODELCHECKING;
	}
    	f->containsTemporal = x->containsTemporal;
    	f->validCTLStateFormula = x->validCTLStateFormula;
    	f->validCTLPathFormula = false;
    	f->validLTLFormula = x->validLTLFormula;
    }];

f=Conjunction(x,y), f=Disjunction(x,y) -> [temporal: x y 
    {
	if(not (x->containsTemporal || y->containsTemporal))
	{
        	f->type = FORMULA_INITIAL;
	}
	else
	{
        	f->type = FORMULA_BOOLEAN;
	}
	f->cannotcompute=x->cannotcompute || y -> cannotcompute;
	if(f->cannotcompute)
	{
		f->type = FORMULA_MODELCHECKING;
	}
    	f->containsTemporal = (x->containsTemporal || y->containsTemporal);
   	f->validCTLStateFormula = (x->validCTLStateFormula && y->validCTLStateFormula);
    	f->validCTLPathFormula = false;
    	f->validLTLFormula = (x->validLTLFormula && y->validLTLFormula);
    }];

f=AllPath(x) -> [temporal: x 
    {
        f->cannotcompute = (x -> cannotcompute || !(x->validCTLPathFormula));
        if(! f->cannotcompute)
        {
	    if(x->validLTLFormula)
	    {
		f->type = FORMULA_BOTH;
            }
	    else
	    {
		    f->type = FORMULA_CTL;
	    }
        }
        if (f -> cannotcompute && x -> validLTLFormula)
        {
            f->type = FORMULA_LTL;
        }
    	f->containsTemporal = true;
    	f->validCTLStateFormula = x->validCTLPathFormula;
    	f->validCTLPathFormula = false;
    	f->validLTLFormula = false;
    }];

f=ExPath(x) -> [temporal: x 
    {
	f->cannotcompute = (x -> cannotcompute || !(x->validCTLPathFormula));
	if(! f->cannotcompute)
	{
		f->type = FORMULA_CTL;
	}
    	f->containsTemporal = true;
    	f->validCTLStateFormula = x->validCTLPathFormula;
    	f->validCTLPathFormula = false;
    	f->validLTLFormula = false;
    }];

f=Always(x), f=NextState(x) -> [temporal: x 
    {
	f->cannotcompute = (x -> cannotcompute || !(x->validLTLFormula || x->validCTLStateFormula));
	if((not f->cannotcompute)&& x->validLTLFormula)
	{
		f->type = FORMULA_LTL;
	}
    	f->containsTemporal = true;
    	f->validCTLStateFormula = false;
    	f->validCTLPathFormula = x->validCTLStateFormula;
    	f->validLTLFormula = x->validLTLFormula;
    }];

f=Until(x,y), f=Release(x,y) -> [temporal: x y 
    {
	f->cannotcompute = ( x -> cannotcompute || y -> cannotcompute || !((x->validLTLFormula && y -> validLTLFormula)||(x->validCTLStateFormula && y->validCTLStateFormula)));
    	f->containsTemporal = true;
    	f->validCTLStateFormula = false;
    	f->validCTLPathFormula = (x->validCTLStateFormula && y->validCTLStateFormula);
    	f->validLTLFormula = (x->validLTLFormula && y->validLTLFormula);
	if((not f->cannotcompute) && f->validLTLFormula)
	{
		f->type = FORMULA_LTL;
	}
    }];


// \todo: next ones redundant: have no pairs when this view is applied
f=EU(x,y) , f=ER(x,y) -> [temporal: x y 
    {
        f->containsTemporal = true;
        f->validCTLStateFormula = x->validCTLStateFormula && y->validCTLStateFormula;
        f->validCTLPathFormula = false;
        f->validLTLFormula = false;
        f->cannotcompute = (x->cannotcompute || y -> cannotcompute || ! x->validCTLStateFormula || ! y->validCTLStateFormula);
        if(!f->cannotcompute)
        {
            f->type = FORMULA_CTL;
        } 
    }];
    
f=AU(x,y), f=AR(x,y) -> [temporal: x y 
    {
        f->containsTemporal = true;
        f->validCTLStateFormula = x->validCTLStateFormula && y->validCTLStateFormula;
        f->validCTLPathFormula = false;
        f->validLTLFormula = false;
        f->cannotcompute = (x->cannotcompute || y -> cannotcompute || ! x->validCTLStateFormula || ! y->validCTLStateFormula);
        if(!f->cannotcompute)
        {
            if(x -> validLTLFormula && y -> validLTLFormula)
 	    {
		f->type = FORMULA_BOTH;
            }
	    else
	    {
		    f->type = FORMULA_CTL;
	    }
        }  
        if (f -> cannotcompute && x -> validLTLFormula && y -> validLTLFormula)
        {
            f->type = FORMULA_LTL;
        }
    }];

f=EX(x), f=EF(x), f=EG(x) -> [temporal: x 
    {
        f->containsTemporal = true;
        f->validCTLStateFormula = x->validCTLStateFormula;
        f->validCTLPathFormula = false;
        f->validLTLFormula = false;
        f->cannotcompute = (x->cannotcompute || ! x->validCTLStateFormula);
        if(!f->cannotcompute)
        {
            f->type = FORMULA_CTL;
        } 
    }];

f=AX(x), f=AF(x), f=AG(x) -> [temporal: x 
    {
        f->containsTemporal = true;
        f->validCTLStateFormula = x->validCTLStateFormula;
        f->validCTLPathFormula = false;
        f->validLTLFormula = false;
        f->cannotcompute = (x->cannotcompute || ! x->validCTLStateFormula);
        if(!f->cannotcompute)
        {
            if(x->validLTLFormula)
            {
		f->type = FORMULA_BOTH;
            }
            else
            { 
		    f->type = FORMULA_CTL;
            }
        }  
        if (f -> cannotcompute && x -> validLTLFormula)
        {
            f->type = FORMULA_LTL;
        }
    }];



//////////////////////////////////////////////////////////////////////////
%uview internal;

%{ KC_UNPARSE
#include <config.h>
#include <Core/Dimensions.h>
#include <Formula/StatePredicate/AtomicStatePredicate.h>
#include <Formula/StatePredicate/ConjunctionStatePredicate.h>
#include <Formula/StatePredicate/DisjunctionStatePredicate.h>
#include <Formula/StatePredicate/FalsePredicate.h>
#include <Formula/StatePredicate/StatePredicate.h>
#include <Formula/StatePredicate/TruePredicate.h>
#include <Net/Marking.h>
#include <Net/Net.h>

#include <map>
#include <vector>
#include <stack>

// mapping from node ids to factors
std::map<int, int> current;

// mapping from tree node ids to list of intermediate formulas
std::map<int, std::vector<StatePredicate *> > formulas;

// invariant: top is always my parent
std::stack<size_t> id_stack;

// next free id
size_t nextId = 0;
%}

f=StatePredicateFormula(x) -> [internal: {
	// clear all data-structures
	current.clear();
	formulas.clear();
	while (not id_stack.empty())
		id_stack.pop();
	nextId = 0;
    id_stack.push(nextId++);
}
x
{
    nextId = id_stack.top();
    id_stack.pop();
    // return the only entry for the root
    assert(nextId == 0);
    assert(formulas[0].size() == 1);
    f->formula = formulas[0][0];
}];

Conjunction(a,b) -> [internal: {
    id_stack.push(nextId++);
}
a b
{
    // after returning from my childs, I am the top of the stack
    const arrayindex_t myId = id_stack.top();
    id_stack.pop();

    StatePredicate *f = new ConjunctionStatePredicate(formulas[myId].size());
    for (size_t i = 0; i < formulas[myId].size(); ++i)
    {
        assert(formulas[myId][i]);
        ((ConjunctionStatePredicate*)f)->addSub(i, formulas[myId][i]);
    }

    formulas[id_stack.top()].push_back(f);
}];

ConjunctionList(l) -> [internal: {
    id_stack.push(nextId++);
}
l
{
    // after returning from my childs, I am the top of the stack
    const arrayindex_t myId = id_stack.top();
    id_stack.pop();

    StatePredicate *f = new ConjunctionStatePredicate(l->length());
    
    for (int i = 0; i < l->length(); ++i)
    {
        assert(formulas[myId][i]);
        ((ConjunctionStatePredicate*)f)->addSub(i, formulas[myId][i]);
    }

    formulas[id_stack.top()].push_back(f);
}];

Disjunction(a,b) -> [internal: {
    id_stack.push(nextId++);
}
a b
{
    // after returning from my childs, I am the top of the stack
    const arrayindex_t  myId = id_stack.top();
    id_stack.pop();

    StatePredicate *f = new DisjunctionStatePredicate(formulas[myId].size());
    for (size_t i = 0; i < formulas[myId].size(); ++i)
    {
        assert(formulas[myId][i]);
        ((DisjunctionStatePredicate*)f)->addSub(i, formulas[myId][i]);
    }

    formulas[id_stack.top()].push_back(f);
}];

DisjunctionList(l) -> [internal: {
    id_stack.push(nextId++);
}
l
{
    // after returning from my childs, I am the top of the stack
    const arrayindex_t myId = id_stack.top();
    id_stack.pop();

    StatePredicate *f = new DisjunctionStatePredicate(l->length());
    
    for (int i = 0; i < l->length(); ++i)
    {
        assert(formulas[myId][i]);
        ((DisjunctionStatePredicate*)f)->addSub(i, formulas[myId][i]);
    }

    formulas[id_stack.top()].push_back(f);
}];

a=AtomicProposition(NNegation(x=Elementary())) -> [internal: 
{
    formula = x->pred -> copy(NULL);
    formula = formula -> negate();
	
    formulas[id_stack.top()].push_back(formula);    
}];

a=AtomicProposition(x=Elementary()) -> [internal: 
{
    formulas[id_stack.top()].push_back(x->pred->copy(NULL));    
}];


True() -> [internal: {
    StatePredicate *f = new TruePredicate();
    // register formula at parent
    formulas[id_stack.top()].push_back(f);    
}];

False() -> [internal: {
    StatePredicate *f = new FalsePredicate();
    // register formula at parent
    formulas[id_stack.top()].push_back(f);    
}];

Deadlock() -> [internal: {
    StatePredicate *f = new DeadlockPredicate(true);
    // register formula at parent
    formulas[id_stack.top()].push_back(f);    
}];

NoDeadlock() -> [internal: {
    StatePredicate *f = new DeadlockPredicate(false);
    // register formula at parent
    formulas[id_stack.top()].push_back(f);    
}];

//////////////////////////////////////////////////////////////////////////
%uview buechi;


%{ KC_UNPARSE
std::map<uint32_t, std::vector<std::pair<uint32_t,uint32_t> > > transitions ;
std::set<uint32_t> acceptingset;
std::map<uint32_t, StatePredicate*> predicates;
std::map<uint32_t, uint32_t> states_to_interal;

uint32_t get_state_number(uint32_t state){
	if (states_to_interal.find(state) == states_to_interal.end()){
		// get a new state number
		uint32_t new_state_number = states_to_interal.size();
		states_to_interal[state] = new_state_number;
		return new_state_number;
	}
	return states_to_interal[state];
}
%}


b=BuechiNull -> [buechi: { RT::rep->message("NULL BUECHI"); b->automata = 0; }];
b=BuechiAutomaton(trans,accept) -> [buechi:
	trans
	accept
    {
    	//RT::rep->message("REAL BUECHI");
    	
    	// get number of states
    	uint32_t number_of_states = states_to_interal.size();
		
		// construction of the buechi automata
		b->automata = new BuechiAutomata();
		b->automata->cardStates = number_of_states;

		//RT::rep->message("transfer predicates");
    	// transfer all state predicates
    	
    	//RT::rep->message("transfer transitions");
    	// build the transition-data-structure
    	b->automata->cardTransitions = new uint32_t[number_of_states]();
    	b->automata->nextstate = new uint32_t*[number_of_states]();
    	b->automata->guard = new StatePredicate**[number_of_states]();
    	for (uint32_t i = 0; i < number_of_states; i++){
    		b->automata->cardTransitions[i] = transitions[i].size();
    		b->automata->nextstate[i] = new uint32_t[transitions[i].size()]();
    		b->automata->guard[i] = new StatePredicate*[transitions[i].size()]();
    		for (uint32_t j = 0; j < transitions[i].size(); j++){
    			b->automata->guard[i][j] = predicates[transitions[i][j].first];
    			b->automata->nextstate[i][j] = transitions[i][j].second;
    		}
    	}
    	
    	//RT::rep->message("transfer accepting states");
        // produce a list for the accepting states
        b->automata->isStateAccepting = new bool[number_of_states]();
        for (__typeof__(acceptingset.begin()) i = acceptingset.begin(); i != acceptingset.end(); i++)
        	b->automata->isStateAccepting[*i] = true;
    }
];


ExpandedBuechiRule(from, formula, to) -> [buechi:
    {
    
    	formula->unparse(myprinter, kc::internal);
        StatePredicate* result = formula->formula;
        //formula->free(true);
        
        
        
    	//RT::rep->message("RULE %d -> %d (%d)", from, to,formula->formula);
    	// get a number for the formula
    	uint32_t formula_number = (uint32_t)(predicates.size());
    	predicates[formula_number] = result;
    	// get numbers for the from and to state
    	uint32_t from_number = get_state_number(from->value);
    	uint32_t to_number = get_state_number(to->value);
    	// insert transition
        transitions[from_number].push_back(std::make_pair(formula_number, to_number));
    } 
];

EmptyBuechiRules() -> [buechi:];

BuechiRules(lrules, rrules) -> [buechi:
    lrules
    rrules
];

AcceptingState(state) -> [buechi:
    {
        acceptingset.insert(get_state_number(state->value));
    }
];

AcceptingSet(set1, set2) -> [buechi:
  set1
  set2
];

EmptyAcceptingSet() -> [buechi:];



//////////////////////////////////////////////////////////////////////////
%uview ctl;
%{ KC_UNPARSE

#include <Core/Dimensions.h>
#include <Exploration/StatePredicateProperty.h>
#include <Formula/CTL/AUFormula.h>
#include <Formula/CTL/AXFormula.h>
#include <Formula/CTL/AtomicFormula.h>
#include <Formula/CTL/CTLFormula.h>
#include <Formula/CTL/ConjunctionFormula.h>
#include <Formula/CTL/DisjunctionFormula.h>
#include <Formula/CTL/EUFormula.h>
#include <Formula/CTL/EXFormula.h>
#include <Formula/CTL/NotFormula.h>
#include <Net/Net.h>

#include <map>
#include <vector>
#include <stack>

// mapping from node ids to factors
std::map<int, int> ctl_current;

// mapping from tree node ids to list of intermediate formulas
std::map<int, std::vector<CTLFormula_p> > ctl_formulas;

// invariant: top is always my parent
std::stack<size_t> ctl_id_stack;

// next free id
size_t ctl_nextId = 0;

// needs dfs-storage;
size_t ctl_dfs_id = 0;

// needs cached results id
size_t ctl_result_cache_id = 0;

// temporal storage to be able to include the LessEqualStatePredicate into an AtomicFormula
StatePredicateProperty* spp;
%}

//EINSTIEG

f=AtomicProposition(x=Elementary()) -> [ctl: {
            AtomicFormula* af = new AtomicFormula();
            af->inner = new StatePredicateProperty(x->pred->copy(NULL));
	  ctl_formulas[ctl_id_stack.top()].push_back(af);
}];
f=AtomicProposition(NNegation(x=Elementary())) -> [ctl: {
            AtomicFormula* af = new AtomicFormula();
            af->inner = new StatePredicateProperty(x->pred->copy(NULL)->negate());
	  ctl_formulas[ctl_id_stack.top()].push_back(af);
}];

f=AtomicProposition(True()) -> [ctl: {
 AtomicFormula* af = new AtomicFormula();
            af->inner = new StatePredicateProperty(new TruePredicate());
          ctl_formulas[ctl_id_stack.top()].push_back(af);
}];

f=AtomicProposition(False()) -> [ctl: {
 AtomicFormula* af = new AtomicFormula();
            af->inner = new StatePredicateProperty(new FalsePredicate());
          ctl_formulas[ctl_id_stack.top()].push_back(af);
}];

f=AtomicProposition(Deadlock()) -> [ctl: {
 AtomicFormula* af = new AtomicFormula();
            af->inner = new StatePredicateProperty(new DeadlockPredicate(1));
          ctl_formulas[ctl_id_stack.top()].push_back(af);
}];

f=AtomicProposition(NoDeadlock()) -> [ctl: {
 AtomicFormula* af = new AtomicFormula();
            af->inner = new StatePredicateProperty(new DeadlockPredicate(0));
          ctl_formulas[ctl_id_stack.top()].push_back(af);
}];

f=StatePredicateFormula(x) -> [ctl: {
    f->unparse(myprinter, kc::temporal);
    // making sure that correct information about contained temporal operators is available at every node. Note that this information is _inevietable_ in order to unparse CTL-Formulas correctly!
    ctl_id_stack.push(ctl_nextId++);
}
x
{
    ctl_nextId = ctl_id_stack.top();
    ctl_id_stack.pop();

    // return the only entry for the root
    assert(ctl_nextId == 0);
    f->ctl_formula = ctl_formulas[0][0];
    f->ctl_formula->payloadsize = ctl_dfs_id*SIZEOF_STATENUMBER_T + (2*ctl_result_cache_id+7)/8;
}];

//Boolsche Operatoren

// Negation can only be in as consequence of workarounds, see phase 5 in rewrite
Negation(a) -> [ctl: {
    ctl_id_stack.push(ctl_nextId++);
}
a
{
    // after returning from my childs, I am the top of the stack
    const arrayindex_t myId = ctl_id_stack.top();
    ctl_id_stack.pop();

    assert(ctl_formulas[myId].size() == 1);
    NotFormula *nf = new NotFormula(ctl_formulas[myId][0]);
    ctl_formulas[ctl_id_stack.top()].push_back(nf);
}];

f=Conjunction(a,b) provided (f->containsTemporal) -> [ctl: {
    ctl_id_stack.push(ctl_nextId++);
}
a b
{
    // after returning from my childs, I am the top of the stack
    const arrayindex_t myId = ctl_id_stack.top();
    ctl_id_stack.pop();

    ConjunctionFormula *cf = new ConjunctionFormula(ctl_formulas[myId]);
    ctl_formulas[ctl_id_stack.top()].push_back(cf);
}];

f=Disjunction(a,b) provided (f->containsTemporal) -> [ctl: {
    ctl_id_stack.push(ctl_nextId++);
}
a b
{
    // after returning from my childs, I am the top of the stack
    const arrayindex_t myId = ctl_id_stack.top();
    ctl_id_stack.pop();

    DisjunctionFormula *df = new DisjunctionFormula(ctl_formulas[myId]);
    ctl_formulas[ctl_id_stack.top()].push_back(df);
}];

//CTL-Operatoren

AX(x) -> [ctl: {
    ctl_id_stack.push(ctl_nextId++);
}
x
{
	// after returning from my childs, I am the top of the stack
	const arrayindex_t myId = ctl_id_stack.top();
	ctl_id_stack.pop();

	assert(ctl_formulas[myId].size() == 1);
	AXFormula* axf = new AXFormula(ctl_formulas[myId][0]);
	
	ctl_formulas[ctl_id_stack.top()].push_back(axf);
}];

AU(a,b) -> [ctl: {
    ctl_id_stack.push(ctl_nextId++);
	const size_t myDFSId = ctl_dfs_id++;
	const size_t myCacheId = ctl_result_cache_id++;
}
a b
{
    // after returning from my childs, I am the top of the stack
    const arrayindex_t myId = ctl_id_stack.top();
    ctl_id_stack.pop();

	assert(ctl_formulas[myId].size() == 2);
	AUFormula* auf = new AUFormula(ctl_formulas[myId][0], ctl_formulas[myId][1]);
	auf->index = 2 * myCacheId;
	auf->dfsindex = myDFSId * SIZEOF_STATENUMBER_T;

	ctl_formulas[ctl_id_stack.top()].push_back(auf);
}];

EX(x) -> [ctl: {
    ctl_id_stack.push(ctl_nextId++);
}
x
{
	// after returning from my childs, I am the top of the stack
	const arrayindex_t myId = ctl_id_stack.top();
	ctl_id_stack.pop();

	assert(ctl_formulas[myId].size() == 1);
	EXFormula* exf = new EXFormula(ctl_formulas[myId][0]);

	ctl_formulas[ctl_id_stack.top()].push_back(exf);
}];

EU(a,b) -> [ctl: {
    ctl_id_stack.push(ctl_nextId++);
	const size_t myDFSId = ctl_dfs_id++;
	const size_t myCacheId = ctl_result_cache_id++;
}
a b
{
    // after returning from my childs, I am the top of the stack
    const arrayindex_t myId = ctl_id_stack.top();
    ctl_id_stack.pop();

	assert(ctl_formulas[myId].size() == 2);
	EUFormula* euf = new EUFormula(ctl_formulas[myId][0], ctl_formulas[myId][1]);
	euf->index = 2 * myCacheId;
	euf->dfsindex = myDFSId * SIZEOF_STATENUMBER_T;

	ctl_formulas[ctl_id_stack.top()].push_back(euf);
}];



//////////////////////////////////////////////////////////////////////////
%uview ltl;

%{ KC_UNPARSE
#include <Core/Runtime.h>
#include <Frontend/Parser/ast-system-rk.h>
#include <vector>

std::vector<LTLTree_p> ltlstack;
%}


f=StatePredicateFormula(x) -> [ltl:
	x
	{
		// put top of stack into memoy
		f->ltl_tree = bin_simpler(push_negation(tl_nn(NOT, ltlstack.back(), NULL)));
		//f->ltl_tree = ltlstack.back();
	}];

f=AtomicProposition(NNegation(x=Elementary()))  -> [ltl:
	{
		// treat this as atomic in Buchi automaton
        StatePredicate * result = x->pred -> copy(NULL);
	result = result -> negate();
        
        // produce a new string
        int num;
        char_p sym = produce_next_string(&num);
        // I know what i am doing, the current sub-tree is an atomic propositon
        predicateMap[num] = result;
        LTLTree_p newTree = tl_nn(PREDICATE, NULL, NULL);
        newTree->sym = tl_lookup(sym);
        newTree = bin_simpler(newTree);
        //RT::rep->message("SYM %x %x",newTree,newTree->sym);
        assert(newTree->sym);
        ltlstack.push_back(newTree);
	}
];

f=AtomicProposition(x=Elementary())  -> [ltl:
	{
		// treat this as atomic in Buchi automaton
        StatePredicate * result = x->pred->copy(NULL);
        
        // produce a new string
        int num;
        char_p sym = produce_next_string(&num);
        // I know what i am doing, the current sub-tree is an atomic propositon
        predicateMap[num] = result;
        LTLTree_p newTree = tl_nn(PREDICATE, NULL, NULL);
        newTree->sym = tl_lookup(sym);
        newTree = bin_simpler(newTree);
        //RT::rep->message("SYM %x %x",newTree,newTree->sym);
        assert(newTree->sym);
        ltlstack.push_back(newTree);
	}
];

f=AtomicProposition(True())  -> [ltl:
	{
		// treat this as atomic in Buchi automaton
        StatePredicate * result = new TruePredicate();
        
        // produce a new string
        int num;
        char_p sym = produce_next_string(&num);
        // I know what i am doing, the current sub-tree is an atomic propositon
        predicateMap[num] = result;
        LTLTree_p newTree = tl_nn(PREDICATE, NULL, NULL);
        newTree->sym = tl_lookup(sym);
        newTree = bin_simpler(newTree);
        //RT::rep->message("SYM %x %x",newTree,newTree->sym);
        assert(newTree->sym);
        ltlstack.push_back(newTree);
	}
];

f=AtomicProposition(Deadlock())  -> [ltl:
	{
		// treat this as atomic in Buchi automaton
        StatePredicate * result = new DeadlockPredicate(true);
        
        // produce a new string
        int num;
        char_p sym = produce_next_string(&num);
        // I know what i am doing, the current sub-tree is an atomic propositon
        predicateMap[num] = result;
        LTLTree_p newTree = tl_nn(PREDICATE, NULL, NULL);
        newTree->sym = tl_lookup(sym);
        newTree = bin_simpler(newTree);
        //RT::rep->message("SYM %x %x",newTree,newTree->sym);
        assert(newTree->sym);
        ltlstack.push_back(newTree);
	}
];

f=AtomicProposition(NoDeadlock())  -> [ltl:
	{
		// treat this as atomic in Buchi automaton
        StatePredicate * result = new DeadlockPredicate(false);
        
        // produce a new string
        int num;
        char_p sym = produce_next_string(&num);
        // I know what i am doing, the current sub-tree is an atomic propositon
        predicateMap[num] = result;
        LTLTree_p newTree = tl_nn(PREDICATE, NULL, NULL);
        newTree->sym = tl_lookup(sym);
        newTree = bin_simpler(newTree);
        //RT::rep->message("SYM %x %x",newTree,newTree->sym);
        assert(newTree->sym);
        ltlstack.push_back(newTree);
	}
];

f=AtomicProposition(False())  -> [ltl:
	{
		// treat this as atomic in Buchi automaton
        StatePredicate * result = new FalsePredicate();
        
        // produce a new string
        int num;
        char_p sym = produce_next_string(&num);
        // I know what i am doing, the current sub-tree is an atomic propositon
        predicateMap[num] = result;
        LTLTree_p newTree = tl_nn(PREDICATE, NULL, NULL);
        newTree->sym = tl_lookup(sym);
        newTree = bin_simpler(newTree);
        //RT::rep->message("SYM %x %x",newTree,newTree->sym);
        assert(newTree->sym);
        ltlstack.push_back(newTree);
	}
];

Eventually(x) -> [ltl: 
	x
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(U_OPER, tl_nn(TRUE, NULL, NULL), a)));
	}
 ];
Always(x)     -> [ltl:
	x
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(V_OPER, tl_nn(FALSE, NULL, NULL), a)));
	}
];
NextState(x)   -> [ltl:
	x
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(NEXT, a, NULL)));
	}
];
Until(phi,psi) -> [ltl: 
	phi
	psi
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		LTLTree_p b = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(U_OPER, b, a)));
	}
];

Release(phi,psi) -> [ltl: 
	phi
	psi
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		LTLTree_p b = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(V_OPER, b, a)));
	}
];

Negation(x) -> [ltl:
	x
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(NOT, a, NULL)));
	}
];
Conjunction(x, y) -> [ltl: 
	x
	y
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		LTLTree_p b = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(AND, a, b)));
	}
];
Disjunction(x, y) -> [ltl:
	x
	y
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		LTLTree_p b = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(OR, a, b)));
	}
];

//////////////////////////////////////////////////////////////////////////
%uview toplevelboolean;

// This view is for translating the top level Boolean operators into
//a hierarchy of tasks.

%{ KC_UNPARSE
#include <Core/Runtime.h>
#include <Frontend/Parser/ast-system-rk.h>
#include <vector>
#include <Planning/Task.h>
#include <Planning/ConjunctionTask.h>
#include <Planning/DisjunctionTask.h>
#include <Planning/LeafTask.h>
%}

f=StatePredicateFormula(x) -> [toplevelboolean: x
{
	f->task = x->task;
}];
f=Conjunction(x,y) provided(f->type != FORMULA_INITIAL) -> [toplevelboolean: x y 
{
	f->task = new ConjunctionTask(x->task,y->task);
}];

f=Disjunction(x,y) provided(f->type != FORMULA_INITIAL) -> [toplevelboolean: x y 
{
	f->task = new DisjunctionTask(x->task,y->task);
}];

f=AtomicProposition(*) ,
f=Negation(*),
f=Conjunction(*, *) provided(f->type == FORMULA_INITIAL),
f=Disjunction(*, *) provided(f->type == FORMULA_INITIAL) ,
f=AllPath(*) ,
f=ExPath(*) ,
f=Always(*),
f=Eventually(*),
f=NextState(*),
f=Until(*,*) ,
f=Release(*,*),
f=EU(*,*) ,
f=AR(*,*) ,
f=ER(*,*),
f=AU(*,*),
f=AF(*) ,
f=AG(*),
f=AX(*),
f=EX(*),
f=EF(*),
f=EG(*) -> [toplevelboolean:
	{
		f -> task = new LeafTask(f);
	}];

//////////////////////////////////////////////////////////////////////////
%uview compound;

// This view is for translating a compound formula into 
//a hierarchy of tasks.

%{ KC_UNPARSE
#include <Core/Runtime.h>
#include <Frontend/Parser/ast-system-rk.h>
#include <vector>
#include <Planning/Task.h>
#include <Planning/CompoundTask.h>
#include <Planning/LeafTask.h>
%}


f=Compound(x,y) -> [compound: x y 
{
	f->task = new CompoundTask(x->task,y->task);
}];

f=CompBound() -> [compound:
	{
		f -> task = new LeafTask(f);
	}];

f=StatePredicateFormula(*) -> [compound:
	{
		f -> task = new LeafTask(f);
	}];


f=AtomicProposition(*) ,
f=Negation(*),
f=AllPath(*) ,
f=ExPath(*) ,
f=Always(*),
f=Eventually(*),
f=NextState(*),
f=Until(*,*) ,
f=Release(*,*),
f=EU(*,*) ,
f=AR(*,*) ,
f=ER(*,*),
f=AU(*,*),
f=AF(*) ,
f=AG(*),
f=AX(*),
f=EX(*),
f=EF(*),
f=EG(*) -> [compound:
	{
		f -> task = new LeafTask(f);
	}];

//////////////////////////////////////////////////////////////////////////
%uview detectcompound;

// This view is for separating a compound formula into
// its constituents

%{ KC_UNPARSE
#include <Core/Runtime.h>
#include <Frontend/Parser/ast-system-rk.h>
#include <vector>
#include <Planning/Task.h>
#include <Planning/CompoundTask.h>
%}


f=Compound(x,y) -> [detectcompound: x y 
{
	f->type = FORMULA_COMPOUND;
}];

/*************************************************************************/
/*                   High Level Net                                      */
/*************************************************************************/

%uview hl_staticanalysis;

Net(c,d,p,m,t) -> [hl_staticanalysis: c d p m t]; 

EmptyDefinitionsList() -> [hl_staticanalysis: ];
DefinitionsList(l,d) -> [hl_staticanalysis: l d];

Constant(t,c,a,e) -> [hl_staticanalysis: t c a e 
{
	/* check whether assigned expression is constant */
	if(! (e->is_constant))
	{
		/* todo: error message */
	}
 	/* todo: determine required type for c from t and a */
	/* todo: check compatibility between required and inferred type */
	/* todo: check whether value of e is within boundaries */
	/* todo: create constant object */
	
}
];
Sort(t,c,a) -> [hl_staticanalysis: t c 
{
	a -> inherited_type = t;
}
a 
{
	/* todo: create type object from a -> type */
}
];
Function(t,c,a,f,b) -> [hl_staticanalysis: t c 
{
	a -> inherited_type = t;
}
a f b
{
	/* todo: create return type from a -> type*/
	/* todo: determine parameter list with types */
        /* todo: create function object */
}
];

TypeBool() -> [hl_staticanalysis: 
{
	size = 2;
	is_finite = true;
	is_scalar = true;
	/* todo: create type object */
}
];

TypeEnum(i) -> [hl_staticanalysis:  i
{
	size = i -> nrvalues;
	if(size == 0)
	{
		/* todo: error message */
	}
	is_finite = true;
	is_scalar = true;
	/* todo: create type object */
}
];

TypeIdent(c) -> [hl_staticanalysis:
{
	/* todo: retrieve type object from c */
	/* todo: set is_finite, is_scalar, and size */
}
];

TypeIntInterval(e,f) -> [hl_staticanalysis: e f 
{
	if(!(e->is_constant && f->is_constant))
	{
		/* todo: error message */
	}
	if(!(e -> is_scalar && f -> is_scalar))
	{
		/* todo: error message */
	}
	arrayindex_t left = *(reinterpret_cast<arrayindex_t *>(e->value));
	arrayindex_t right = *(reinterpret_cast<arrayindex_t *>(f->value));
	if(right < left)
	{
		/* todo: error message */
	}
	is_finite = true;
	is_scalar = true;
	size = right - left +1;
}
];
TypeStruct(s) -> [hl_staticanalysis: s
{
	size = s -> size;
	is_finite = s -> is_finite;
	is_scalar = false;
}
];
TypeInt() -> [hl_staticanalysis:
{
	is_finite = false;
	is_scalar = true;
	size = 0; /* irrelevant */
}
];
TypeMultiset(t) -> [hl_staticanalysis: t
{
	if(! (t->is_finite))
	{
		/* todo: error message */
	}
	is_finite = false;
	is_scalar = false;
	size = 0; /* irrelevant */
}
];
TypeBlack()  -> [hl_staticanalysis: 
{
	is_finite = true;
	is_scalar = true;
	size = 1;
}
];

EmptyOptNumber() -> [hl_staticanalysis:
{
	value = ARRAYINDEX_T_MAX;
	set = false;
}
];

OptNumber(i) -> [hl_staticanalysis:
{
	value = i-> value;
	set = true;
}
];

EmptyIdentList() -> [hl_staticanalysis: 
{
	maxvalue = 0;
	nrvalues = 0;
}
];

IdentList(i,d) -> [hl_staticanalysis: i d
{
	nrvalues = i -> nrvalues + 1;
	if(d -> value <= i -> maxvalue)
	{
		/* todo: error message */
	}
	if(!(d -> set))
	{
		d -> value = i -> maxvalue + 1;
	}
	/* todo: record d -> value in symboltable of symbol d -> c */
	maxvalue = d -> value;
	
}
];

Idents(c,o) -> [hl_staticanalysis: c o
{
	value = o -> value;
	set = o -> set;
}
];

EmptyStructTypeList() -> [hl_staticanalysis:
{
	size = 0;
	nrcomponents = 0;
	is_finite = true;
}
];
StructTypeList(l,s) -> [hl_staticanalysis: l s
{
	size = l -> size * s -> size;
	nrcomponents = l -> nrcomponents + 1;
	is_finite = (l -> is_finite) && (s -> is_finite);
}
];

StructType(t,c,a) -> [hl_staticanalysis: t c 
{
	a -> inherited_type = t;
}
a
{
	size = t -> size * a -> nrelements;
	is_finite = t -> is_finite;
	is_scalar = false;
}
];

VarOrArray(c,a) -> [hl_staticanalysis: 
{
	a -> inherited_type = inherited_type;
}
c a 
{
	type = a -> type;
}
];


EmptyArrayList() -> [hl_staticanalysis:
{
	nrdimensions = 0;
	nrelements = 1;
	type = inherited_type;
}
];
ArrayList(a,e) -> [hl_staticanalysis: 
{
	a -> inherited_type = inherited_type;
}
a e 
{
	if(!(e -> is_constant))
	{
		/* todo: error message */
	}
	if(e -> value == 0)
	{
		/* todo: error message */
	}
	nrdimensions = a -> nrdimensions + 1;
	// todo: nrelements = (a -> nrelements) * reinterpret_cast<int>(e -> value[0]);
	// todo: type = kc::TypeArray(a -> type, e -> value);
	type -> size = nrelements * a -> type -> size;
	type -> is_finite = a -> type -> is_finite;
	type -> is_scalar = false;
	/* todo: test for arrays with too many elements */
}
];
/*
functionParametersList:
  EmptyFunctionParametersList()
| FunctionParametersList(functionParameters functionParametersList)
;

functionParameters:
  FunctionParameters(type varOrArray)
;

expression:
  ExprLeftvalue(leftvalue)
| ExprInitializerList(initializerList)
| AssignEqual(leftvalue expression)
| AssignPlus(leftvalue expression)
| AssignMinus(leftvalue expression)
| AssignTimes(leftvalue expression)
| AssignDivide(leftvalue expression)
| AssignMod(leftvalue expression)
| IncrementVal(leftvalue)
| DecrementVal(leftvalue)
| ValIncrement(leftvalue)
| ValDecrement(leftvalue)
| PositiveExpr(expression)
| NegativeExpr(expression)
| ExprAddition(expression expression)
| ExprSubtraction(expression expression)
| ExprMultiplication(expression expression)
| ExprDivision(expression expression)
| ExprModulo(expression expression)
| ExprNumber(integer)
| NotExpr(expression)
| ExprAnd(expression expression)
| ExprOr(expression expression)
| ExprTrue()
| ExprFalse()
| ExprEquivalent(expression expression)
| ExprNotEqual(expression expression)
| ExprLessThan(expression expression)
| ExprGreaterThan(expression expression)
| ExprLessOrEqual(expression expression)
| ExprGreaterOrEqual(expression expression)
| FunctionCall(casestring expressionlist)
| ExprCommaSeparated(expressionlist)
| ExprOtherIf(expression expression expression)
| ExprAll(type)
{ checkName check = UNDEF; }
;


leftvalue:
  LeftValIdent(casestring)       
| LeftValBrackets(leftvalue expression)
| LeftValDot(leftvalue casestring)
;

expressionlist:
   EmptyExpressionList()
|  ExpressionList(expression expressionlist)
;

initializerList:
  ExprInBraces(expressionlist)
| ExprInBracesColon(expression expression expressionListColon)
;

expressionListColon:
  EmptyExpressionListColon()
| ExpressionListColon(expression expression expressionListColon)
;

placeblocklist:
  EmptyPlaceBlockList()
| PlaceBlockList(placeblock placeblocklist)
;

optSafe:
  EmptySafe()
| Safe(integer)
;

place:
  Place(casestring)
;

placelist:
  EmptyPlaceList()
| PlaceList(place placelist)
;

placeblock:
  PlaceBlock(optSafe type placelist)
;

marking:
  EmptyMarking()
| Marking(casestring expression marking)
;

transition:
  EmptyTransition()
| Transition(casestring fairness variable guard identExprList identExprList transition)
;

fairness:
  EmptyFairness()
| WeakFair()
| StrongFair()
;

variable:
  EmptyVariable()
| Variable(type varOrArrayList variable)
;

varOrArrayList:
  EmptyVarOrArrayList()
| VarOrArrayList(varOrArray varOrArrayList)
;

guard:
 Guard(expression)
;


identExprList:
  EmptyIdentExprList()
| IdentExprList(casestring expression identExprList)
;

block:
  Block(declarationOrStatement)
;

declarationOrStatement:
  EmptyDeclarationOrStatement()
| DeclOrStatemDeclaration(declaration declarationOrStatement)
| DeclOrStatemStatement(statement declarationOrStatement)
;

declaration: 
  Declaration(type varOrArrayList)
;

statement:
  StatementBlock(block)
| StatementExprSemicolon(expression)
| StatementIf(expression statement statement)
| StatementWhile(expression statement)
| StatementDoWhile(statement expression)
| StatementForExpr(expression expression expression statement)
| StatementForTypeExpr(type casestring expression expression expression statement)
| StatementForIdentColon(casestring expression statement)
| StatementForAll(type casestring type)
| StatementSwitch(expression switchCaseList)
| StatementBreak()
| StatementContinue()
| StatementReturn(expression)
| StatementSkip()
;

switchCase:
  SwitchCase(expression statement)
| SwitchDefault(statement)
;

switchCaseList:
  EmptySwitchCaseList()
| SwitchCaseList(switchCase switchCaseList)
;
*/

///////////////////////////////////////////////////////////////
%uview visible;

// Marks transitions as visible that may change value of any subformula

StatePredicateFormula(x) -> [visible: 
{
	for(arrayindex_t i = 0; i < Net::Card[TR]; i++)
	{
		Transition::Visible[i] = false;
	}
} x
{
	containsNext = x -> containsNext;
}];
Compound(x,y) -> [visible: x y
{
	containsNext = (x -> containsNext || y -> containsNext);
}];
//ComputeBound(LessEqualAtomicProposition(x,y)) -> [visible: x y {containsNext = true;}];

AtomicProposition(x=Elementary()) -> [visible: {x -> pred -> setVisible();}];
AtomicProposition(NNegation(x=Elementary())) -> [visible: {x -> pred -> setVisible();}];
Negation(x) -> [visible: x {containsNext = x -> containsNext;}];
Conjunction(x, y) -> [visible:  x  y {containsNext = x -> containsNext || y -> containsNext;}];
Disjunction(x, y) -> [visible: x y {containsNext = x -> containsNext || y -> containsNext;}];
AllPath(x) -> [visible: x {containsNext = x -> containsNext;}];
ExPath(x)  -> [visible: x {containsNext = x -> containsNext;}];
Always(x)     -> [visible: x {containsNext = x -> containsNext;}];
Eventually(x) -> [visible: x {containsNext = x -> containsNext;}];
NextState(x) -> [visible: x {containsNext = true;}];
Until(phi,psi) -> [visible: phi  psi {containsNext = phi->containsNext || psi -> containsNext;}];
Release(phi,psi) -> [visible: phi psi {containsNext = phi->containsNext || psi->containsNext;}];
EU(x,y) -> [visible: x y {containsNext = x -> containsNext || y -> containsNext;}];
AR(x,y) -> [visible: x y {containsNext = x -> containsNext || y -> containsNext;}];
ER(x,y) -> [visible: x y {containsNext = x -> containsNext || y -> containsNext;}];
AU(x,y) -> [visible: x y {containsNext = x -> containsNext || y -> containsNext; }];
AF(x)   -> [visible:  x {containsNext = x -> containsNext;}];
AG(x)   -> [visible: x {containsNext = x -> containsNext;}];
AX(x)   -> [visible: x {
containsNext = true;}];
EX(x)   -> [visible: x {containsNext = true;}];
EF(x)   -> [visible: x {containsNext = x -> containsNext;}];
EG(x)   -> [visible: x {containsNext = x -> containsNext;}];

ConjunctionList(l) -> [visible: l {containsNext = l -> containsNext;}];
ConstConjunction_list(h, NiltConjunction_list()) -> [visible: h {containsNext = h -> containsNext;}];
ConstConjunction_list(h, l) -> [visible: h l {containsNext = h -> containsNext || l -> containsNext;}];

DisjunctionList(l) -> [visible: l {containsNext = l -> containsNext;}];
ConstDisjunction_list(h, NiltDisjunction_list()) -> [visible: h {containsNext = h -> containsNext;}];
ConstDisjunction_list(h, l) -> [visible: h l {containsNext = h -> containsNext || l -> containsNext;}];


True() -> [visible:  {containsNext = false;}];
False() -> [visible:  {containsNext = false;}];
Deadlock() -> [visible: 
{
	for(arrayindex_t i = 0; i < Net::Card[TR];i++)
	{
		Transition::Visible[i] = true;
	}
	containsNext = false;
}
];
NoDeadlock() -> [visible: 
{
	for(arrayindex_t i = 0; i < Net::Card[TR];i++)
	{
		Transition::Visible[i] = true;
	}
	containsNext = false;
}
];


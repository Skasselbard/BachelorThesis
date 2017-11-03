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

%{ KC_REWRITE_HEADER
/// variable counting the rule applications in the rewrite rule sets
extern size_t rule_applications;
%}

%{ KC_REWRITE /* code redirection */
#include <Frontend/Parser/ast-system-rk.h>
#include <CoverGraph/CoverGraph.h>
#include <Net/Marking.h>
#include <Formula/StatePredicate/DeadlockPredicate.h>
#include <Formula/StatePredicate/StatePredicate.h>
#include <Formula/StatePredicate/MagicNumber.h>
#include <Net/Net.h>

/*
Uncomment the next line to define a debug function that traces all matches in
the rewriting process.
*/
//#define KC_TRACE_REWRITE_RESULT(VIEW,FILE,LINE,NODE) { RT::rep->status("rewrite %s:%d", FILE, LINE); }

/*!
Count the rule applications
*/
size_t rule_applications = 0;
#define KC_TRACE_REWRITE_MATCH(VIEW,FILE,LINE,NODE) { ++rule_applications; }

%}

/*****************************************************
 * FORMULA REWRITING PHASE I: REMOVE SYNTACTIC SUGAR *
 *****************************************************/

/* This phase consists of several rule sets to be executed 
   sequentially. Each rule set is responsible for removing
   one of the following elements:
   - double arrows
   - single arrows
   - xor
   Applying all rule sets in the given order results in a 
   formula tree not containing any of these features.
*/

%rview goodbye_doublearrows;

// (x <=> y)    ->   (x OR !y) AND (!x OR y)
Equivalence(x, y) -> <goodbye_doublearrows: 
      Conjunction(Disjunction(x,Negation(y)), Disjunction(Negation(x),y))>;

%rview goodbye_singlearrows;

// (x => y)    ->   (!x OR y)
Implication(x, y) -> <goodbye_singlearrows: Disjunction(Negation(x), y)>;

%rview goodbye_xor;

// (x XOR y)   ->   ((x AND !y) OR (!x AND y))
ExclusiveDisjunction(x, y) -> <goodbye_xor: Disjunction(Conjunction(x, Negation(y)), Conjunction(Negation(x), y))>;

/*********************************************************************
 * PHASE 3: APPLY LOGICAL TAUTOLOGIES AND MOVE CTL* TO CTL OPERATORS *
 *********************************************************************/

/*
This phase consists of just one rule set since all rule applications may
make other rules applicable again. For better readability, we sort the
rules by various themes.
*/

// Theme 1: Boolean operators vs. TRUE/FALSE
// These are first since tree gets smaller
// *****************************************

%rview tautology;

// Atomic TRUE/FALSE
f=AtomicProposition(Elementary()) provided(f->shape == AT_TRUE) -> <tautology: AtomicProposition(True())>;

f=AtomicProposition(Elementary()) provided(f->shape == AT_FALSE) -> <tautology: AtomicProposition(False())>;

f=AtomicProposition(x=Elementary()) provided(x->pred->magicnumber == MAGIC_NUMBER_FALSE) -> <tautology: AtomicProposition(False())>;

f=AtomicProposition(x=Elementary()) provided(x->pred->magicnumber == MAGIC_NUMBER_TRUE) -> <tautology: AtomicProposition(True())>;

f=AtomicProposition(NNegation(x=Elementary())) provided(x->pred->magicnumber == MAGIC_NUMBER_FALSE) -> <tautology: AtomicProposition(True())>;

f=AtomicProposition(NNegation(x=Elementary())) provided(x->pred->magicnumber == MAGIC_NUMBER_TRUE) -> <tautology: AtomicProposition(False())>;

f=AtomicProposition(Elementary()) provided((f->shape == AT_DL) && ((DeadlockPredicate*)(f->formula))->sign) -> <tautology: AtomicProposition(Deadlock())>;

f=AtomicProposition(Elementary()) provided((f->shape == AT_DL) && !(((DeadlockPredicate*)(f->formula))->sign)) -> <tautology: AtomicProposition(NoDeadlock())>;

f=AtomicProposition(Elementary()) provided((f->shape == AT_COMP) && (((AtomicStatePredicate*)(f->formula))->cardPos == 0) && (((AtomicStatePredicate*)(f->formula))->threshold >= 0)) -> <tautology: AtomicProposition(True())>;

f=AtomicProposition(Elementary()) provided((f->shape == AT_COMP) && (((AtomicStatePredicate*)(f->formula))->cardNeg == 0) && (((AtomicStatePredicate*)(f->formula))->threshold < 0)) -> <tautology: AtomicProposition(False())>;

// AND + TRUE/FALSE
Conjunction(a, AtomicProposition(True())) -> <tautology: a >;
Conjunction(AtomicProposition(True()), a) -> <tautology: a >;
Conjunction(*, f = AtomicProposition(False())) -> <tautology: f >;
Conjunction(f = AtomicProposition(False()), *) -> <tautology: f >;

// OR + TRUE/FALSE
Disjunction(a, AtomicProposition(False())) -> <tautology: a >;
Disjunction(AtomicProposition(False()), a) -> <tautology: a >;
Disjunction(*, t=AtomicProposition(True())) -> <tautology: t >;
Disjunction(t=AtomicProposition(True()), *) -> <tautology: t >;

// Theme 2: Temporal operators vs. TRUE/FALSE
// ******************************************

// F TRUE -> TRUE
Eventually(x=AtomicProposition(True())) -> <tautology: x>;

// F FALSE -> FALSE
Eventually(x=AtomicProposition(False())) -> <tautology: x>;

// G TRUE -> TRUE
Always(x=AtomicProposition(True())) -> <tautology: x>;

// G FALSE -> FALSE
Always(x=AtomicProposition(False())) -> <tautology: x>;

// X TRUE -> TRUE
//NextState(x=AtomicProposition(True())) -> <tautology: x>;

// X FALSE -> FALSE
// NextState(x=AtomicProposition(False())) -> <tautology: x>;

// E TRUE -> TRUE
ExPath(x=AtomicProposition(True())) -> <tautology: x>;

// E FALSE -> FALSE
ExPath(x=AtomicProposition(False())) -> <tautology: x>;

// A TRUE -> TRUE
AllPath(x=AtomicProposition(True())) -> <tautology: x>;

// A FALSE -> FALSE
AllPath(x=AtomicProposition(False())) -> <tautology: x>;

// x U TRUE <-> TRUE
Until(*,y=AtomicProposition(True())) -> <tautology: y>;

// TRUE U x <-> F x
Until(AtomicProposition(True()), x) -> <tautology: Eventually(x)>;

// x R TRUE -> TRUE
Release(*,y=AtomicProposition(True())) -> <tautology: y>;

// TRUE R x <-> x
Release(AtomicProposition(True()), x) -> <tautology: x>;

// x U FALSE -> FALSE
Until(*,y=AtomicProposition(False())) -> <tautology: y>;

// FALSE U x <-> x
Until(AtomicProposition(False()), x) -> <tautology: x>;

// FALSE R x <-> G x
Release(AtomicProposition(False()), x) -> <tautology: Always(x)>;

// x R FALSE <-> FALSE
Release(*,y=AtomicProposition(False())) -> <tautology: y>;

// Theme 3: Temporal operators vs. deadlock
// ****************************************

// F nodeadlock -> nodeadlock
Eventually(x=AtomicProposition(NoDeadlock())) -> <tautology: x>;

// G deadlock -> deadlock
Always(x=AtomicProposition(Deadlock())) -> <tautology: x>;

// deadlock U x -> x 
Until(AtomicProposition(Deadlock()),x) -> <tautology: x>;

// nodeadlock U x -> F x
Until(AtomicProposition(NoDeadlock()),x) -> <tautology: Eventually(x)>;

// nodeadlock R x -> x 
Release(AtomicProposition(NoDeadlock()),x) -> <tautology: x>;

// deadlock R x -> G x
Release(AtomicProposition(Deadlock()),x) -> <tautology: Always(x)>;

// A deadlock -> deadlock
AllPath(x=AtomicProposition(Deadlock())) -> <tautology: x>;

// E deadlock -> deadlock
ExPath(x=AtomicProposition(Deadlock())) -> <tautology: x>;

// A nodeadlock -> nodeadlock
AllPath(x=AtomicProposition(NoDeadlock())) -> <tautology: x>;

// E nodeadlock -> nodeadlock
ExPath(x=AtomicProposition(NoDeadlock())) -> <tautology: x>;


// Theme 4: Deadlock vs. Boolean
// *****************************

// The following collapse rules rely on a right-associative grouping 
// of AND/OR chains.
// There are rules that take care of this. These rules appear, however
// after the collapse rules. This way, collapsing gets priority over
// sorting and makes rewriting more efficient.

Disjunction(AtomicProposition(NoDeadlock()),AtomicProposition(Deadlock())) -> <tautology: AtomicProposition(True())>;
Disjunction(AtomicProposition(Deadlock()),AtomicProposition(NoDeadlock())) -> <tautology: AtomicProposition(True())>;
Disjunction(AtomicProposition(Deadlock()),x=AtomicProposition(Deadlock())) -> <tautology: x>;
Disjunction(AtomicProposition(NoDeadlock()),x=AtomicProposition(NoDeadlock())) -> <tautology: x>;

Conjunction(AtomicProposition(NoDeadlock()),AtomicProposition(Deadlock())) -> <tautology: AtomicProposition(False())>;
Conjunction(AtomicProposition(Deadlock()),AtomicProposition(NoDeadlock())) -> <tautology: AtomicProposition(False())>;
Conjunction(AtomicProposition(Deadlock()),x=AtomicProposition(Deadlock())) -> <tautology: x>;
Conjunction(AtomicProposition(NoDeadlock()),x=AtomicProposition(NoDeadlock())) -> <tautology: x>;

Disjunction(AtomicProposition(NoDeadlock()),Disjunction(AtomicProposition(Deadlock()),*)) -> <tautology: AtomicProposition(True())>;
Disjunction(AtomicProposition(Deadlock()),Disjunction(AtomicProposition(NoDeadlock()),*)) -> <tautology: AtomicProposition(True())>;
Disjunction(AtomicProposition(NoDeadlock()),x=Disjunction(AtomicProposition(NoDeadlock()),*)) -> <tautology: x>;
Disjunction(AtomicProposition(Deadlock()),x=Disjunction(AtomicProposition(Deadlock()),*)) -> <tautology: x>;

Conjunction(AtomicProposition(NoDeadlock()),Conjunction(AtomicProposition(Deadlock()),*)) -> <tautology: AtomicProposition(False())>;
Conjunction(AtomicProposition(Deadlock()),Conjunction(AtomicProposition(NoDeadlock()),*)) -> <tautology: AtomicProposition(False())>;
Conjunction(AtomicProposition(NoDeadlock()),x=Conjunction(AtomicProposition(NoDeadlock()),*)) -> <tautology: x>;
Conjunction(AtomicProposition(Deadlock()),x=Conjunction(AtomicProposition(Deadlock()),*)) -> <tautology: x>;

// Theme 5: Normalize chains of Boolean operators
// **********************************************

// (x AND y) AND z -> x AND (y AND z)
Conjunction(Conjunction(x,y),z) -> <tautology: Conjunction(x,Conjunction(y,z))>;

// (x OR y) OR z -> x OR (y OR z)
Disjunction(Disjunction(x,y),z) -> <tautology: Disjunction(x,Disjunction(y,z))>;

/* priorities are not yet implemented
// sort OR chains by "priority" of subformulas
Disjunction(x,y) provided(x->priority > y->priority) -> <tautology: Disjunction(y,x)>;
Disjunction(x,Disjunction(y,z)) provided(x->priority > y->priority) -> <tautology: Disjunction(y,Disjunction(x,z))>;

// sort AND chains by "priority" of subformulas
Conjunction(x,y) provided(x->priority > y->priority) -> <tautology: Conjunction(y,x)>;
Conjunction(x,Conjunction(y,z)) provided(x->priority > y->priority) -> <tautology: Conjunction(y,Conjunction(x,z))>;
*/

// Theme 6: Push down negations.
// In effect, resulting formulas have no negations at all.
// *******************************************************

// Negations of atomic propositions. After phase 2, only <= and > remain.

// NOT (p > n)   ->    p <= n
//Negation(AtomicProposition(GreaterAtomicProposition(p,n))) -> <tautology: AtomicProposition(LessEqualAtomicProposition(p, n))>;

// NOT (p <= n)   ->    p > n
//Negation(AtomicProposition(LessEqualAtomicProposition(p,n))) -> <tautology: AtomicProposition(GreaterAtomicProposition(p, n))>;

// NOT TRUE  ->  FALSE
Negation(AtomicProposition(True())) -> <tautology: AtomicProposition(False())>;

// NOT FALSE  ->  TRUE
Negation(AtomicProposition(False())) -> <tautology: AtomicProposition(True())>;

// NOT deadlock -> nodeadlock
Negation(AtomicProposition(Deadlock())) -> <tautology: AtomicProposition(NoDeadlock())>;

// NOT nodeadlock -> deadlock
Negation(AtomicProposition(NoDeadlock())) -> <tautology: AtomicProposition(Deadlock())>;

// NOT Fireable -> Unfireable
//Negation(AtomicProposition(Fireable(t))) -> <tautology: AtomicProposition(Unfireable(t))>;

// NOT Unfireable -> Fireable
//Negation(AtomicProposition(Unfireable(t))) -> <tautology: AtomicProposition(Fireable(t))>;

// NOT Elementary -> NegElementary
Negation(AtomicProposition(x=Elementary())) -> <tautology: AtomicProposition(NNegation(x))>;

// NOT NegElementary -> Elementary
Negation(AtomicProposition(NNegation(x=Elementary()))) -> <tautology: AtomicProposition(x)>;

//NNegation(Fireable(t)) -> <tautology: Unfireable(t)>;
//NNegation(Unfireable(t)) -> <tautology: Fireable(t)>;
NNegation(Deadlock()) -> <tautology: NoDeadlock()>;
NNegation(NoDeadlock()) -> <tautology: Deadlock()>;
NNegation(True()) -> <tautology: False()>;
NNegation(False()) -> <tautology: True()>;
NNegation(NNegation(x)) -> <tautology: x>;
// Negations of Boolean operators

// NOT(NOT(x))  ->  x
Negation(Negation(x)) -> <tautology: x>;

// NOT (x AND y)  ->    (NOT x OR NOT y)
Negation(Conjunction(x, y)) -> <tautology: Disjunction(Negation(x), Negation(y))>;

// NOT (x OR y)   ->    (NOT x AND NOT y)
Negation(Disjunction(x, y)) -> <tautology: Conjunction(Negation(x), Negation(y))>;

// Negations and temporal operators

// NOT A   <->   E NOT
Negation(AllPath(x)) -> <tautology: ExPath(Negation(x))>;

// NOT E   <->   A NOT
Negation(ExPath(x)) -> <tautology: AllPath(Negation(x))>;

// NOT G   <->   F NOT
Negation(Always(x)) -> <tautology: Eventually(Negation(x))>;

// NOT F   <->   G NOT
Negation(Eventually(x)) -> <tautology: Always(Negation(x))>;

// NOT X   <->   X NOT
Negation(NextState(x)) -> <tautology: NextState(Negation(x))>;

// NOT (x U y) -> (NOT x R NOT y) 
Negation(Until(x,y)) -> <tautology: Release(Negation(x), Negation(y))>;

// NOT (x R y) -> (NOT x U NOT y) 
Negation(Release(x,y)) -> <tautology: Until(Negation(x), Negation(y))>;

// Theme 7: Repeated temporal operators
// ************************************

// FF a        <->  Fa
Eventually(a=Eventually(*)) -> <tautology: a>;

// AA x       <->  A x
AllPath(a=AllPath(*)) -> <tautology: a>;

// AE x       <->  E x
AllPath(a=ExPath(*)) -> <tautology: a>;

// EA x       <->  A x
ExPath(a=AllPath(*)) -> <tautology: a>;

// EE x       <->  E x
ExPath(a=ExPath(*)) -> <tautology: a>;

// FGF x       <->  GF x
Eventually(a=Always(Eventually(*))) -> <tautology: a>;

// GFG x       <->  FG x
Always(a=Eventually(Always(*))) -> <tautology: a>;

// AGAG x   <-> AG x
AllPath(Always(a=AllPath(Always(*)))) -> <tautology: a>;

// AFAF x   <-> AF x
AllPath(Eventually(a=AllPath(Eventually(*)))) -> <tautology: a>;

// EFEF x   <-> EF x
ExPath(Eventually(a=ExPath(Eventually(*)))) -> <tautology: a>;

// AGEFAGEF x <-> AGEF x
AllPath(Always(ExPath(Eventually(a=AllPath(Always(ExPath(Eventually(*)))))))) -> <tautology: a>;

// EFAGEFAG x <-> EFAG x
ExPath(Eventually(AllPath(Always(a=ExPath(Eventually(AllPath(Always(*)))))))) -> <tautology: a>;

// X (x U y) <-> (Xx U Xy)
Until(NextState(x),NextState(y)) -> <tautology: NextState(Until(x,y))>;

// (x U (x U y)) -> (x U y)
// Until(x, a=Until(x,*)) -> <tautology: a>;

// x U x -> x
Until(a=AtomicProposition(x=Elementary()),AtomicProposition(y=Elementary())) provided(x->pred->magicnumber==y->pred->magicnumber) -> <tautology: a>;

// x U -x -> F -x
Until(AtomicProposition(x=Elementary()),a=AtomicProposition(y=Elementary())) provided(x->pred->magicnumber== - y->pred->magicnumber) -> <tautology: Eventually(a)>;

// x U F y -> F y
Until(x,y=Eventually(z)) -> <tautology: y>;

// x U GF y -> GF y
Until(x,y=Always(Eventually(z))) -> <tautology: y>;

// GF x U y -> F (y & GF x)
Until(z=Always(Eventually(x)),y) -> <tautology: Conjunction(y,z)>;

// FG x U y -> F (y & FG x)
Until(z=Eventually(Always(x)),y) -> <tautology: Conjunction(y,z)>;

// Gx U y -> x U (y & G x)
Until(z=Always(x),y) -> <tautology: Until(x,Conjunction(y,z))>;

// F(x U y) -> F y
Eventually(Until(x,y)) -> <tautology: Eventually(y)>;

// G(x U y) -> GF y & G(x|y)
Always(Until(x,y)) -> <tautology: Conjunction(Always(Eventually(y)),Always(Conjunction(x,y)))>;

// (y U x) U x -> y U x
Until(Until(y,a=AtomicProposition(x=Elementary())),AtomicProposition(z=Elementary())) provided(x -> pred -> magicnumber == z -> pred -> magicnumber) -> <tautology: Until(y,a)>;

// (x U y) U x -> y U x
Until(Until(a=AtomicProposition(x=Elementary()),y),AtomicProposition(z=Elementary())) provided(x -> pred -> magicnumber == z -> pred -> magicnumber) -> <tautology: Until(y,a)>;

// x U (y U x) -> y U x
Until(a=AtomicProposition(x=Elementary()),Until(y,AtomicProposition(z=Elementary()))) provided(x -> pred -> magicnumber == z -> pred -> magicnumber) -> <tautology: Until(y,a)>;

// x & Gx -> Gx
Conjunction(AtomicProposition(x=Elementary()),y=Always(AtomicProposition(z=Elementary()))) provided(x->pred->magicnumber == z -> pred->magicnumber) -> <tautology: y>;

// x | Gx -> x
Disjunction(y=AtomicProposition(x=Elementary()),Always(AtomicProposition(z=Elementary()))) provided(x->pred->magicnumber == z -> pred->magicnumber) -> <tautology: y>;

// x & Fx -> x
Conjunction(y=AtomicProposition(x=Elementary()),Eventually(AtomicProposition(z=Elementary()))) provided(x->pred->magicnumber == z -> pred->magicnumber) -> <tautology: y>;

// x | Fx -> Fx
Disjunction(AtomicProposition(x=Elementary()),y=Eventually(AtomicProposition(z=Elementary()))) provided(x->pred->magicnumber == z -> pred->magicnumber) -> <tautology: y>;

// y & (x U y) -> y
// y | (x U y) -> x U y
// x & G -x -> false
// x | F -x -> true
// -x & (x U y) -> y
// x & y U x -> x
// -x | (y U x) -> true

// Theme 8: Temporal operators vs. Boolean
// We try to push temporal operators down. This way, individual explorations
// act on simpler formulas. On top level, we can split into several 
// individual runs.
// *************************************************************************

// temporal operators need to move up, for finding a path quantifier

// (F a) OR (F b)  ->  F (a OR b)
Disjunction(Eventually(x),Eventually(y)) -> <tautology: Eventually(Disjunction(x,y))>;

// (G a) AND (G b)  ->  G (a AND b)
Conjunction(Always(x),Always(y)) -> <tautology: Always(Conjunction(x,y))>;

// (X a) AND (X b)  ->  X (a AND b)
Conjunction(NextState(x), NextState(y)) -> <tautology: NextState(Conjunction(x,y))>;

// (X a) OR (X b)   <->   X (a OR b)  
Disjunction(NextState(x), NextState(y)) -> <tautology: NextState(Disjunction(x,y))>;

// path quantifiers need to move down, for finding a temporal operator

// E (a OR b) -> (E a) OR (E b)
ExPath(Disjunction(x,y)) -> <tautology: Disjunction(ExPath(x),ExPath(y))>;

// A (a AND b) -> (A a) AND (A b)
AllPath(Conjunction(x, y)) -> <tautology: Conjunction(AllPath(x),AllPath(y))>;

// matched CTL operators can go either way. Pushing up means less recursive
//   searches on more global subformulas. Additionally, chance are that rewriting
//   in subformulas finds tautologies. Pushing down means more recursive
//   searches on more local subformulas. Current decision: push down
// hope for boolean operators at top level --> many separate problems

// AG (a AND b) -> (AG a) AND (AG b)
AllPath(Always(Conjunction(a,b))) -> <tautology:Conjunction(AllPath(Always(a)),AllPath(Always(b)))>;

// EF (a OR b) -> (EF a) OR (EF b)
ExPath(Eventually(Disjunction(a,b))) -> <tautology: Disjunction(ExPath(Eventually(a)),ExPath(Eventually(b)))>;

// AX (a AND b) -> (AX a) AND (AX b)
AllPath(NextState(Conjunction(a,b))) -> <tautology: Conjunction(AllPath(NextState(a)),AllPath(NextState(b))) >;

// EX (a OR b) -> (EX a) OR (EX b)
ExPath(NextState(Disjunction(a,b))) -> <tautology: Disjunction(ExPath(NextState(a)),ExPath(NextState(b)))>;

/**************************************
 * Phase 3a: REMOVE EMPTY QUANTIFIERS *
 **************************************/

// A quantifier is "empty" if it does not contain any temporal operators.

%rview emptyquantifiers;

ExPath(x) provided (not x->containsTemporal) -> <emptyquantifiers: x>;
AllPath(x) provided (not x->containsTemporal) -> <emptyquantifiers: x>;


/*************************************************
 * Phase 4: REMOVE SINGLE ROOT TEMPORAL OPERATOR *
 *************************************************/

/*
4a. singletemporal
These rules eleminate the root temporal operator(s) to perform certain checks
with dedicated routines rather than general purpose CTL/LTL algorithms.

The following transformations are made:

- EF f -> f      (in case of reachability; algorithm: reachability)
- AG f -> NOT f  (in case of invariance; algorithm: reachability)
- AG EF f -> f   (in case of liveness; algorithm: currently only cover)
- EF AG f -> NOT f  (algorithm: liveness)
- EF AG EF f -> f  (algorithm: tscc)
- AG EF AG f -> NOT f  (algorithm: tscc)

4b. simpleneg
We need to repeat some negation rules as we may introduce negation in 4a.
These rules do not change associativity of AND/OR, so the normal form
of AND/OR chains is preserved.

4c. booleanlists
Transform Chains of AND/OR into lists
*/

%rview singletemporal;

Negation(Negation(y)) -> <singletemporal: y>;
// Liveness
// AG EF f -> f
StatePredicateFormula(AllPath(Always(ExPath(Eventually(y))))) provided(not y->containsTemporal)-> <singletemporal: StatePredicateFormula(y)>;

// Impossibility : (KW: cannot happen since negation is not there)
// AG NOT f -> NOT EF f -> f   (the outmost negation is handled by LoLA)
// StatePredicateFormula(AllPath(Always(Negation(y)))) provided(not y->containsTemporal)-> <singletemporal: StatePredicateFormula(y)>;

// Reachability
// EF f -> f
StatePredicateFormula(ExPath(Eventually(y))) provided(not y->containsTemporal)-> <singletemporal: StatePredicateFormula(y)>;

// Invariance
// AG f -> NOT EF NOT f -> NOT f   (the outmost negation is handled by LoLA)
StatePredicateFormula(AllPath(Always(y))) provided(not y->containsTemporal)-> <singletemporal: StatePredicateFormula(Negation(y))>;


// Possible Invariance
// EF AG f -> NOT AG EF NOT f -> NOT f   (the outmost negation is handled by LoLA)
StatePredicateFormula(ExPath(Eventually(AllPath(Always(y))))) provided(not y->containsTemporal)-> <singletemporal: StatePredicateFormula(Negation(y))>;

// Possible Liveness
// EF AG EF f -> f  
StatePredicateFormula(ExPath(Eventually(AllPath(Always(ExPath(Eventually(y))))))) provided(not y->containsTemporal)-> <singletemporal: StatePredicateFormula(y)>;

// Live Invariance
// AG EF AG f -> NOT EF AG EF NOT f -> NOT f  (the outmost negation is handled by LoLA)
StatePredicateFormula(AllPath(Always(ExPath(Eventually(AllPath(Always(y))))))) provided(not y->containsTemporal)-> <singletemporal: StatePredicateFormula(Negation(y))>;

%rview simpleneg;

Negation(Negation(x)) -> <simpleneg: x>;
// NOT (x AND y)  ->    (NOT x OR NOT y)
Negation(Conjunction(x, y)) -> <simpleneg: Disjunction(Negation(x), Negation(y))>;

// NOT (x OR y)   ->    (NOT x AND NOT y)
Negation(Disjunction(x, y)) -> <simpleneg: Conjunction(Negation(x), Negation(y))>;

// NOT TRUE  ->  FALSE
Negation(AtomicProposition(True())) -> <simpleneg: AtomicProposition(False())>;

// NOT FALSE  ->  TRUE
Negation(AtomicProposition(False())) -> <simpleneg: AtomicProposition(True())>;

// NOT deadlock -> nodeadlock
Negation(AtomicProposition(Deadlock())) -> <simpleneg: AtomicProposition(NoDeadlock())>;

// NOT nodeadlock -> deadlock
Negation(AtomicProposition(NoDeadlock())) -> <simpleneg: AtomicProposition(Deadlock())>;

// NOT Fireable -> Unfireable
//Negation(AtomicProposition(Fireable(t))) -> <simpleneg: AtomicProposition(Unfireable(t))>;

// NOT Unfireable -> Fireable
//Negation(AtomicProposition(Unfireable(t))) -> <simpleneg: AtomicProposition(Fireable(t))>;

Negation(AtomicProposition(x=Elementary())) -> <simpleneg: AtomicProposition(NNegation(x))>;
Negation(AtomicProposition(NNegation(x=Elementary()))) -> <simpleneg: AtomicProposition(x)>;

%rview booleanlists;

// collect conjunctions in lists
Conjunction(x,y) -> <booleanlists: ConjunctionList(ConstConjunction_list(x, ConstConjunction_list(y, NiltConjunction_list())))>;
ConstConjunction_list(ConjunctionList(x), y) -> <booleanlists: concat(x,y) >;

// collect disjunctions in lists
Disjunction(x,y) -> <booleanlists: DisjunctionList(ConstDisjunction_list(x, ConstDisjunction_list(y, NiltDisjunction_list())))>;
ConstDisjunction_list(DisjunctionList(x), y) -> <booleanlists: concat(x,y) >;

/***************************************************************************
 * PHASE 5: (CTL only) path quantifier + temporal operator -> CTL operator *
 ***************************************************************************/

%rview ctloperators;

// as long as not all CTL operators have their own search classes, we
// use workarounds. Introduced negations are not replaced as this may
// cause formula blowup (and CTL model checking can handle negation) 
// \todo: remove workarounds as soon as CTL ist complete.

// workaround for: ExPath(Eventually(x)) -> <ctloperators: EF(x)>;
ExPath(Eventually(x)) -> <ctloperators: EU(AtomicProposition(True()),x)>;

ExPath(NextState(x)) -> <ctloperators: EX(x)>;

// workaround for: ExPath(Always(x)) -> <ctloperators: EG(x)>;
ExPath(Always(x)) -> <ctloperators: Negation(AU(AtomicProposition(True()),Negation(x)))>;

ExPath(Until(x,y)) -> <ctloperators: EU(x,y)>;

// workaround for: ExPath(Release(x,y)) -> <ctloperators: ER(x,y)>;
ExPath(Release(x,y)) -> <ctloperators: Negation(AU(Negation(x),Negation(y)))>;

// workaround for: AllPath(Eventually(x)) -> <ctloperators: AF(x)>;
AllPath(Eventually(x)) -> <ctloperators: AU(AtomicProposition(True()),x)>;

AllPath(NextState(x)) -> <ctloperators: AX(x)>;

// workaround for: AllPath(Always(x)) -> <ctloperators: AG(x)>;
AllPath(Always(x)) -> <ctloperators: Negation(EU(AtomicProposition(True()),Negation(x)))>;

AllPath(Until(x,y)) -> <ctloperators: AU(x,y)>;

// workaround for: AllPath(Release(x,y)) -> <ctloperators: AR(x,y)>;
AllPath(Release(x,y)) -> <ctloperators: Negation(EU(Negation(x),Negation(y)))>;


%rview rbuechi;

// expands all transition rules to triples (from,formula,to)
BuechiRule(from,TransitionRules(lrules,rrules)) -> <rbuechi: BuechiRules(BuechiRule(from,lrules),BuechiRule(from,rrules))>;
BuechiRule(from,TransitionRule(formula,to)) -> <rbuechi: ExpandedBuechiRule(from,formula,to)>;
BuechiRule(*,EmptyTransitionRules()) -> <rbuechi: EmptyBuechiRules()>;
BuechiRules(a,EmptyBuechiRules()) -> <rbuechi: a>;
BuechiRules(EmptyBuechiRules(),a) -> <rbuechi: a>;


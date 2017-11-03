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
\author Karsten
\status new

\brief class implementation for atomic state predicates
*/

#include <config.h>
#include <Core/Handlers.h>
#include <Formula/StatePredicate/AtomicStatePredicate.h>
#include <Formula/StatePredicate/AtomicBooleanPredicate.h>
#include <Net/LinearAlgebra.h>
#include <Net/Net.h>
#include <Net/Transition.h>
#include <Net/NetState.h>
#include <Net/Marking.h>
#include <CoverGraph/CoverGraph.h>
#include <Formula/FormulaInfo.h>
#include <Formula/StatePredicate/MagicNumber.h>

/*!
\brief creates a state predicate with a formal sum of p places with positive
factor, n places with negative factor, and constant k particular places are
added using addPos and addNeg

\param p  number of places with positive factor
\param n  number of places with negative factor
\param k  constant

\todo Schleifen behandeln - können evtl. rausgenommen werden
*/
AtomicStatePredicate::AtomicStatePredicate(arrayindex_t p, arrayindex_t n, int k) :
    posPlaces(new arrayindex_t[p]), negPlaces(new arrayindex_t[n]),
    posMult(new capacity_t[p]), negMult(new capacity_t[n]), cardPos(p),
    cardNeg(n), up(NULL), cardUp(0), threshold(k), sum(0), original(true)
{
    parent = NULL;
    magicnumber = MagicNumber::assign();
    literals = 1;
}

AtomicStatePredicate::AtomicStatePredicate(Term * T) :
    up(NULL), cardUp(0), threshold(0), sum(0), original(true)
{
    literals = 1;
    int64_t * mult = new int64_t[Net::Card[PL]];
    parent = NULL;
    cardPos = 0;
    cardNeg = 0;
    for(arrayindex_t i = 0; i < Net::Card[PL];i++)
    {
	mult[i] = 0;
    }
    while(T)
    {
	if(T->place == Net::Card[PL])
	{
		// constant factor
		if(T->mult == OMEGA)
		{
			if(threshold == -OMEGA)
			{
                    		RT::rep->message("%s: addition -oo + oo",
              			RT::rep->markup(MARKUP_WARNING, "error").str());
				RT::rep->abort(ERROR_SYNTAX);
			}
			threshold = OMEGA;
		}
		else if(T->mult == -OMEGA)
		{
			if(threshold == OMEGA)
			{
                    		RT::rep->message("%s: addition -oo + oo",
              			RT::rep->markup(MARKUP_WARNING, "error").str());
				RT::rep->abort(ERROR_SYNTAX);
			}
			threshold = -OMEGA;
	
		}
		else if((threshold != OMEGA) && (threshold != -OMEGA))
		{
			threshold += T->mult;
		}
		// else: adding omega with nonomega -> no change
	}
	else
	{
		// real place
		if(T->mult == OMEGA)
		{
			if(mult[T->place] == -OMEGA)
			{
                    		RT::rep->message("%s: addition -oo + oo",
              			RT::rep->markup(MARKUP_WARNING, "error").str());
				RT::rep->abort(ERROR_SYNTAX);
			}
			mult[T->place] = OMEGA;
		}
		else if(T->mult == -OMEGA)
		{
			if(mult[T->place] == OMEGA)
			{
                    		RT::rep->message("%s: addition -oo + oo",
              			RT::rep->markup(MARKUP_WARNING, "error").str());
				RT::rep->abort(ERROR_SYNTAX);
			}		
			mult[T->place] = OMEGA;
		}
		else if((mult[T->place] != OMEGA) && (mult[T->place] != -OMEGA))
		{
				mult[T->place] += T->mult;
		}
		// else: adding omega with nonomega -> no change
	}
	    Term * oldT = T;
	    T = T -> next;
	    oldT -> next = NULL;
	    delete oldT;
    }
    cardPos = 0;
    cardNeg = 0;
    for(arrayindex_t i = 0; i < Net::Card[PL];i++)
    {
	if(mult[i] > 0)
	{
		cardPos++;
  	}
	else if(mult[i] < 0)
	{
		cardNeg++;
	}
    }
    posPlaces = new arrayindex_t [cardPos];
    posMult = new arrayindex_t [cardPos];
    negPlaces = new arrayindex_t [cardNeg];
    negMult = new arrayindex_t [cardNeg];
    cardPos = 0;
    cardNeg = 0;
    for(arrayindex_t i = 0; i < Net::Card[PL];i++)
    {
	if(mult[i] > 0)
	{
		posPlaces[cardPos] = i;
		posMult[cardPos++] = mult[i];
  	}
	else if(mult[i] < 0)
	{
		negPlaces[cardNeg] = i;
		negMult[cardNeg++] = -mult[i];
	}
    }
    threshold = - threshold; // threshould must be put to the other side
			    // of the inequation
    delete[] mult;

    // assign magic number
    if((cardPos == 0) && (threshold >= 0))
    {
	magicnumber = MAGIC_NUMBER_TRUE;
    }
    else if((cardNeg == 0) && (threshold < 0))
    {
	magicnumber = MAGIC_NUMBER_FALSE;
    }
    else if((cardPos == 1) && (cardNeg == 0) && (threshold == 0) && (posMult[0] == 1))
    {
	magicnumber = MAGIC_NUMBER_EMPTY(posPlaces[0]);  // p<1
    }
    else if((cardPos == 0) &&(cardNeg == 1) && (threshold == -1) && (negMult[0] == 1))
    {
	magicnumber = MAGIC_NUMBER_MARKED(negPlaces[0]); // p>0
    }
    else
    {
	magicnumber = MagicNumber::assign(); // any predicate
    } 
}

AtomicStatePredicate::AtomicStatePredicate() :
    posPlaces(NULL), negPlaces(NULL), posMult(NULL), negMult(NULL), cardPos(0),
    cardNeg(0), up(NULL), cardUp(0), threshold(0), sum(0), original(true)
{
	magicnumber = MAGIC_NUMBER_TRUE; // 0 <= 0
        literals = 1;
}


AtomicStatePredicate::~AtomicStatePredicate()
{
    if (!original)
    {
        return;
    }
    free( posPlaces);
    free( negPlaces);
    free( posMult);
    free( negMult);
    free(up);
}

/*!
\param i  position in atomic state predicate
\param p  place
\param m  positive factor
*/
void AtomicStatePredicate::addPos(arrayindex_t i, arrayindex_t p, capacity_t m)
{
    assert(i < cardPos);
    posPlaces[i] = p;
    posMult[i] = m;
    if((cardPos == 1) && (cardNeg == 0) && (threshold == 0) && (posMult[0] == 1))
    {
	magicnumber = MAGIC_NUMBER_EMPTY(posPlaces[0]);  // p<1
    }
    else
    {
	magicnumber = MagicNumber::assign(); // any predicate
    } 
}

/*!
\param i  position in atomic state predicate
\param p  place
\param m  negative factor
*/
void AtomicStatePredicate::addNeg(arrayindex_t i, arrayindex_t p, capacity_t m)
{
    assert(i < cardNeg);
    negPlaces[i] = p;
    negMult[i] = m;
    if((cardPos == 0) &&(cardNeg == 1) && (threshold == -1) && (negMult[0] == 1))
    {
	magicnumber = MAGIC_NUMBER_MARKED(negPlaces[0]); // p>0
    }
    else
    {
	magicnumber = MagicNumber::assign(); // any predicate
    } 
}

arrayindex_t AtomicStatePredicate::getUpSet(arrayindex_t *stack, bool *onstack, bool * needEnabled) const
{
    assert(onstack);
    arrayindex_t stackpointer = 0;
    for (arrayindex_t i = 0; i < cardUp; i++)
    {
        arrayindex_t element;
        if (!onstack[element = up[i]])
        {
            onstack[element] = true;
            stack[stackpointer++] = element;
        }
    }
    * needEnabled = false;
    return stackpointer;
}

arrayindex_t AtomicStatePredicate::getDownSet(arrayindex_t *stack, bool *onstack, bool * needEnabled) const
{
    * needEnabled = false;
    assert(onstack);
    arrayindex_t stackpointer = 0;
    for (arrayindex_t i = 0; i < cardDown; i++)
    {
        arrayindex_t element;
        if (!onstack[element = down[i]])
        {
            onstack[element] = true;
            stack[stackpointer++] = element;
        }
    }
    return stackpointer;
}

/*!
If value of this changes, it needs to be propagated to its parent. The
parameter is the change in the formal sum k_1 p_1 + ... + k_n p_n between the
previously considered marking and the current marking. Having a precomputed
value for this change, evaluation of the formula is accelerated.
*/
void AtomicStatePredicate::update(NetState &, int delta)
{
    sum += delta;
    if (sum <= threshold && !value)
    {
        value = true;
        if (parent)
        {
            parent->updateFT(position);
        }
        return;
    }
    if (sum > threshold && value)
    {
        value = false;
        if (parent)
        {
            parent->updateTF(position);
        }
        return;
    }
}

/*!
Evaluation starts top/down, so the whole formula is examined. Evaluation is
done w.r.t. Marking::Current.

\param ns  net state to evaluate the formula
*/
void AtomicStatePredicate::evaluate(NetState &ns)
{
    sum = 0;
    for (arrayindex_t i = 0; i < cardPos; ++i)
    {
        sum += ns.Current[posPlaces[i]] * posMult[i];
    }
    for (arrayindex_t i = 0; i < cardNeg; ++i)
    {
        sum -= ns.Current[negPlaces[i]] * negMult[i];
    }

    value = (sum <= threshold);
}

/*!
Evaluation with Omega starts top/down, so the whole formula is examined. Evaluation is
done w.r.t. Marking::Current.

\param ns  net state to evaluate the formula
*/
void AtomicStatePredicate::evaluateOmega(NetState &ns)
{
    sum = 0;
    unknown = false;
    for (arrayindex_t i = 0; i < cardPos; ++i)
    {
        if (ns.Current[posPlaces[i]] == OMEGA)
        {
            sum = OMEGA;
        }
        else if (sum < OMEGA)
        {
            sum += ns.Current[posPlaces[i]] * posMult[i];
        }
    }
    assert(sum >= 0);
    for (arrayindex_t i = 0; i < cardNeg; ++i)
    {
        if (ns.Current[negPlaces[i]] == OMEGA)
        {
            if (sum == OMEGA && threshold < OMEGA)
            {
                unknown = true;
            }
            else
            {
                sum = -OMEGA;
            }
        }
        else if (sum > -OMEGA && sum < OMEGA)
        {
            sum -= ns.Current[negPlaces[i]] * negMult[i];
        }
    }

    if ((sum == OMEGA || sum == -OMEGA) && threshold < OMEGA && threshold > -OMEGA)
    {
        unknown = true;
    }
    value = (sum <= threshold);
}

arrayindex_t AtomicStatePredicate::countAtomic() const
{
    return 1;
}

arrayindex_t AtomicStatePredicate::collectAtomic(AtomicStatePredicate **c)
{
    c[0] = this;
    return 1;
}


arrayindex_t AtomicStatePredicate::countDeadlock() const
{
    return 0;
}

arrayindex_t AtomicStatePredicate::collectDeadlock(DeadlockPredicate **)
{
    return 0;
}

arrayindex_t AtomicStatePredicate::countFireable() const
{
    return 0;
}

arrayindex_t AtomicStatePredicate::collectFireable(FireablePredicate **)
{
    return 0;
}

void AtomicStatePredicate::initUpSet()
{
	cardUp = 0;
        up = reinterpret_cast<arrayindex_t *>(malloc(Net::Card[TR] * SIZEOF_ARRAYINDEX_T));
}

void AtomicStatePredicate::finitUpSet()
{
    // shrink up array to size actually needed
    up = reinterpret_cast<arrayindex_t *>(realloc(up, cardUp * SIZEOF_ARRAYINDEX_T));
}

void AtomicStatePredicate::addToUpSet(arrayindex_t t)
{
	up[cardUp++] = t;
}

void AtomicStatePredicate::initDownSet()
{
	cardDown = 0;
        down = reinterpret_cast<arrayindex_t *>(malloc(Net::Card[TR] * SIZEOF_ARRAYINDEX_T));
}

void AtomicStatePredicate::finitDownSet()
{
    // shrink up array to size actually needed
    down = reinterpret_cast<arrayindex_t *>(realloc(down, cardDown * SIZEOF_ARRAYINDEX_T));
}

void AtomicStatePredicate::addToDownSet(arrayindex_t t)
{
	down[cardDown++] = t;
}

// LCOV_EXCL_START
bool AtomicStatePredicate::DEBUG__consistency(NetState &ns)
{
    // 1. check sum
    int s = 0;
    for (arrayindex_t i = 0; i < cardPos; i++)
    {
        s += posMult[i] * ns.Current[posPlaces[i]];
    }
    for (arrayindex_t i = 0; i < cardNeg; i++)
    {
        s -= negMult[i] * ns.Current[negPlaces[i]];
    }
    assert(s == sum);
    if (value)
    {
        assert(sum <= threshold);
    }
    else
    {
        assert(sum > threshold);
    }
    /* if (this != top)
     {
         assert(parent);
     }*/
    return true;
}
// LCOV_EXCL_STOP

/*!
\param parent  the parent predicate for the new, copied, object
*/
StatePredicate *AtomicStatePredicate::copy(StatePredicate *parent)
{
    AtomicStatePredicate *af = new AtomicStatePredicate(0, 0, 0);
    af->magicnumber = magicnumber;
    af->value = value;
    af->position = position;
    af->parent = parent;
    // we can copy the pointers, so use the same arrays as they are not changed!
    af->posPlaces = posPlaces;
    af->negPlaces = negPlaces;
    af->posMult = posMult;
    af->negMult = negMult;
    af->cardPos = cardPos;
    af->cardNeg = cardNeg;
    af->threshold = threshold;
    af->sum = sum;
    af->up = up;
    af->cardUp = cardUp;
    af->original = false;
    return af;
}

arrayindex_t AtomicStatePredicate::getSubs(const StatePredicate *const **)
const
{
    return 0;
}

StatePredicate *AtomicStatePredicate::negate()
{
    arrayindex_t tmp;
    arrayindex_t * tmpp;
    tmp = cardPos;
    cardPos = cardNeg;
    cardNeg = tmp;
    tmpp = posPlaces;
    posPlaces = negPlaces;
    negPlaces = tmpp;
    tmpp = posMult;
    posMult = negMult;
    negMult = tmpp;
    threshold = - threshold - 1;
    magicnumber = -magicnumber;
    return this;
}

FormulaInfo *AtomicStatePredicate::getInfo() const
{
    FormulaInfo *Info = new FormulaInfo();
    Info->tag = formula_atomic;
    Info->cardChildren = 0;
    Info->f = const_cast<AtomicStatePredicate *>(this);
    return Info;
}

int AtomicStatePredicate::countSubFormulas() const
{
    return 1;
}

/*!
Reduces all factors and the threshold with the gcd thereof.

\note This function needs to be called after all addPos/addNeg calls are
complete.
*/
void AtomicStatePredicate::reduceFactors()
{
    // make sure there is at least one multiplicity
    assert(cardPos + cardNeg > 0);

    // initialize result value
    int64_t gcd = (cardPos > 0) ? posMult[0] : negMult[0];

    // find gcd of threshold and all multiplicities
    for (arrayindex_t i = 0; i < cardPos; i++)
    {
        gcd = ggt(gcd, posMult[i]);
    }
    for (arrayindex_t i = 0; i < cardNeg; i++)
    {
        gcd = ggt(gcd, negMult[i]);
    }

    // add threshold to the result
    if (threshold != 0)
    {
        gcd = ggt(gcd, threshold);
    }

    assert(gcd);

    // make sure the gcd is positive
    gcd = (gcd < 0) ? -gcd : gcd;

    // apply ggt
    threshold /= gcd;
    for (arrayindex_t i = 0; i < cardPos; i++)
    {
        assert(posMult[i] % gcd == 0);
        posMult[i] /= gcd;
    }
    for (arrayindex_t i = 0; i < cardNeg; i++)
    {
        assert(negMult[i] % gcd == 0);
        negMult[i] /= gcd;
    }
}

char * addNumber(char * text, int64_t num)
{
	char * result = text;
	result = (char *) realloc(result,strlen(result) + 32);
	if(num == OMEGA)
	{
		sprintf(result+strlen(result),"oo");
	}
	else if(num == FINITE)
	{
		sprintf(result+strlen(result)," FINITE ");
	}
	else
	{
		sprintf(result+strlen(result),"%lld",num);
	}
	return result;
}

char * addText(char * text, const char * ttext)
{
	char * result = text;
	result = (char *) realloc(result,strlen(result) + strlen(ttext) + 1);
	sprintf(result+strlen(result),"%s",ttext);
	return result;
}

char * addSummand(char * text, arrayindex_t p, arrayindex_t mult)
{
	char * result = text;
	const char * place = Net::Name[PL][p];
	result = (char *) realloc(result,strlen(result) + strlen(place) + 50);
	if(mult != 1)
	{
		sprintf(result+strlen(result)," %d * ",mult);
	}
	sprintf(result+strlen(result),"%s",place);
	return result;
}

char * AtomicStatePredicate::toString()
{
	char * result = (char *) malloc(sizeof(char));
	result[0] = '\0';
	result = addText(result,"(");


	// build string left of <=
	// - all positive terms
	// - -threshold (if negative)
	// - 0 if none of these

	if(cardPos == 0)
	{
		if(threshold >= 0)
		{
			result = addText(result,"0");
		}
		else
		{
			result = addNumber(result,-threshold);
		}
		
	}
	else
	{
		result = addSummand(result,posPlaces[0],posMult[0]);
		for(arrayindex_t i = 1; i < cardPos;i++)
		{
			result = addText(result," + ");
			result = addSummand(result,posPlaces[i],posMult[i]);
		}
		if(threshold < 0)
		{
			result = addText(result," + ");
			result = addNumber(result,-threshold);
		}
	}
	result = addText(result," <= ");

	// build string  right of <=
	// - all negative terms
	// - threshold (if positive)
	// - 0 if none of these

	if(cardNeg == 0)
	{
		if(threshold <= 0)
		{
			result = addText(result,"0");
		}
		else
		{
			result = addNumber(result,threshold);
		}
		
	}
	else
	{
		result = addSummand(result,negPlaces[0],negMult[0]);
		for(arrayindex_t i = 1; i < cardNeg;i++)
		{
			result = addText(result," + ");
			result = addSummand(result,negPlaces[i],negMult[i]);
		}
		if(threshold > 0)
		{
			result = addText(result," + ");
			result = addNumber(result,threshold);
		}
	}
	result = addText(result,")");
	return result;
	
}

char * AtomicStatePredicate::toCompString()
{
	char * result = (char *) malloc(sizeof(char));
	result[0] = '\0';
	if(cardPos == 0)
	{
		result = addText(result,"0");
	}
	else	
	{
		result = addSummand(result,posPlaces[0],posMult[0]);	
	}
	for(arrayindex_t i = 1; i < cardPos;i++)
	{
		result = addText(result," + ");
		result = addSummand(result,posPlaces[i],posMult[i]);	
	}
	for(arrayindex_t i = 0; i < cardNeg;i++)
	{
		result = addText(result," - ");
		result = addSummand(result,negPlaces[i],negMult[i]);	
	}
	return result;
}
void AtomicStatePredicate::adjust(arrayindex_t old, arrayindex_t nw)
{
	for(arrayindex_t i = 0; i < cardPos; i++)
	{
		if(posPlaces[i] == old) posPlaces[i] = nw;
	}
	for(arrayindex_t i = 0; i < cardNeg; i++)
	{
		if(negPlaces[i] == old) negPlaces[i] = nw;
	}
}

void AtomicStatePredicate::setVisible()
{
	for(arrayindex_t i = 0; i < cardPos; i++)
	{
		arrayindex_t p = posPlaces[i];
		for(arrayindex_t j = 0; j < Net::CardArcs[PL][PRE][p];j++)
		{
			Transition::Visible[Net::Arc[PL][PRE][p][j]] = true;
		}
		for(arrayindex_t j = 0; j < Net::CardArcs[PL][POST][p];j++)
		{
			Transition::Visible[Net::Arc[PL][POST][p][j]] = true;
		}
	}
	for(arrayindex_t i = 0; i < cardNeg; i++)
	{
		arrayindex_t p = negPlaces[i];
		for(arrayindex_t j = 0; j < Net::CardArcs[PL][PRE][p];j++)
		{
			Transition::Visible[Net::Arc[PL][PRE][p][j]] = true;
		}
		for(arrayindex_t j = 0; j < Net::CardArcs[PL][POST][p];j++)
		{
			Transition::Visible[Net::Arc[PL][POST][p][j]] = true;
		}
	}
}

AtomicBooleanPredicate * AtomicStatePredicate::DNF()
{
	// result is singleton conjunction
	AtomicBooleanPredicate * result = new AtomicBooleanPredicate(true);
	result -> addSub(this); // call DNF on a copy of orginal formula,
				// otherwise parent link breaks!!!!!!!!!
	result -> magicnumber = magicnumber; // result is eq to this!
	return result;
}

FormulaStatistics * AtomicStatePredicate::count(FormulaStatistics * fs)
{
	if(magicnumber == MAGIC_NUMBER_TRUE)
	{
		fs -> taut++;
		return fs;
	}
	if(magicnumber == MAGIC_NUMBER_FALSE)
	{
		fs -> cont++;
		return fs;
	}
	fs->comp++;
	fs-> place_references += cardPos;
	fs-> place_references += cardNeg;
	for(arrayindex_t i = 0; i < cardPos;i++)
	{
		if(fs->mentioned_place[posPlaces[i]]) continue;
		fs->mentioned_place[posPlaces[i]] = true;
		fs->visible_places++;
	}
	for(arrayindex_t i = 0; i < cardNeg;i++)
	{
		if(fs->mentioned_place[negPlaces[i]]) continue;
		fs->mentioned_place[negPlaces[i]] = true;
		fs->visible_places++;
	}
	return fs;
}

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

\brief class implementation for disjunction state predicates
*/

#include <Core/Dimensions.h>
#include <Formula/StatePredicate/StatePredicate.h>
#include <Formula/StatePredicate/AtomicBooleanPredicate.h>
#include <Formula/FormulaInfo.h>
#include <Net/Net.h>
#include <Formula/StatePredicate/MagicNumber.h>


AtomicBooleanPredicate::AtomicBooleanPredicate(bool b)
{
	isAnd = b;
	literals = 0;
	sub = NULL;
	cardSub = 0;
	parent = NULL;
	magicnumber = MagicNumber::assign();
}

AtomicBooleanPredicate::~AtomicBooleanPredicate()
{
    for (arrayindex_t i = 0; i < cardSub; i++)
    {
        delete sub[i];
    }
    delete[] sub;
}

void AtomicBooleanPredicate::addSub(StatePredicate *f)
{
    // handle the nobrainers based on mgic numbers
    if(isAnd && (magicnumber == MAGIC_NUMBER_FALSE)) return; // this already contains p&-p
    if((!isAnd) && (magicnumber == MAGIC_NUMBER_TRUE)) return; // this already contains p|-p
    if(isAnd && (f->magicnumber == MAGIC_NUMBER_TRUE)) return; // f does not contribute to conj
    if((!isAnd) && (f->magicnumber == MAGIC_NUMBER_FALSE)) return; // f does not contribute to disj
    if(isAnd && (f->magicnumber == MAGIC_NUMBER_FALSE)) // f makes conj contradiction
    {
	cardSub = 0;
	literals = 0;
	magicnumber = MAGIC_NUMBER_FALSE;
	return;
    }
    if((!isAnd) && (f->magicnumber == MAGIC_NUMBER_TRUE)) // f makes disj tautology
    {
	cardSub = 0;
	literals = 0;
	magicnumber = MAGIC_NUMBER_TRUE;
	return;
    }
    // check using magic number whether new subformula or its negation
    // is already there
    for(arrayindex_t i = 0; i < cardSub; i++)
    {
	if(sub[i]->magicnumber == f -> magicnumber) 
	{
		// f already contained --> does not contribute
		return;
	}
	if(sub[i]->magicnumber == ((-1)* f->magicnumber )) 
	{
		// f introduces contradiction/tautology
		cardSub = 0;
		literals = 0;
		if(isAnd) magicnumber = MAGIC_NUMBER_FALSE;
		else magicnumber = MAGIC_NUMBER_TRUE;
		return;
	}
    }
	
    if(sub)
    {
	sub = reinterpret_cast<StatePredicate **>(realloc(sub,SIZEOF_VOIDP * (cardSub + 1)));
    }
    else
    {
	sub = reinterpret_cast<StatePredicate **>(malloc(SIZEOF_VOIDP*(cardSub+1)));
    }
    f->position = cardSub;
    f->parent = this;
    sub[cardSub++] = f;
    literals += f->literals;
    if(cardSub == 1)
    {
	magicnumber = sub[0]->magicnumber;
    }
    else
    {
	// sub > 1, inequal, not negation of each other -> value unknown
	magicnumber = MagicNumber::assign();
    }
}

void AtomicBooleanPredicate::merge(AtomicBooleanPredicate * other)
{
	assert(isAnd == other->isAnd);
    // get rid of nobrainers
    if(isAnd && (magicnumber == MAGIC_NUMBER_FALSE)) // this alredy contradiction
    {
	return;
    }
    if((!isAnd) && (magicnumber == MAGIC_NUMBER_TRUE)) // this alredy tautology
    {
	return;
    }
    if(isAnd && (other -> magicnumber == MAGIC_NUMBER_FALSE)) // other is contrdiction
    {
	cardSub = 0;
  	literals = 0;
	magicnumber = MAGIC_NUMBER_FALSE;
	return;
    }
    if((!isAnd) && (other -> magicnumber == MAGIC_NUMBER_TRUE)) // other is tautology
    {
	cardSub = 0;
  	literals = 0;
	magicnumber = MAGIC_NUMBER_TRUE;
	return;
    }
    if(isAnd && (magicnumber == MAGIC_NUMBER_TRUE)) // this does not contribute
    {
	cardSub = other->cardSub;
	literals = other -> literals;
	sub = other->sub;
	magicnumber = other->magicnumber;
	return;
    }
    if((!isAnd) && (magicnumber == MAGIC_NUMBER_FALSE)) // this does not contribute
    {
	cardSub = other->cardSub;
	sub = other->sub;
	literals = other -> literals;
	magicnumber = other->magicnumber;
	return;
    }
    if(isAnd && (other->magicnumber == MAGIC_NUMBER_TRUE)) // other does not contribute
    {
	return;
    }
    if((!isAnd) && (other->magicnumber == MAGIC_NUMBER_FALSE)) // other does not contribute
    {
	return;
    }
    if(sub)
    {
	sub = reinterpret_cast<StatePredicate **>(realloc(sub,SIZEOF_VOIDP * (cardSub + other->cardSub )));
    }
    else
    {
	sub = reinterpret_cast<StatePredicate **>(malloc(SIZEOF_VOIDP * (cardSub+other->cardSub)));
    }
    int oldCard = cardSub;
    for(arrayindex_t i = 0; i < other->cardSub;i++)
    {
	int result = 0;
	for(arrayindex_t j = 0; j < oldCard;j++)
	{
		if(sub[j]->magicnumber ==  other->sub[i]->magicnumber) continue;
		if(sub[j]->magicnumber == -other->sub[i]->magicnumber)
		{
			cardSub = 0;
			literals = 0;
			if(isAnd) magicnumber = MAGIC_NUMBER_FALSE; // sub&-sub
			else magicnumber = MAGIC_NUMBER_TRUE; // sub | -sub
			return;
		};
	}
     	sub[cardSub] = other->sub[i];
    	sub[cardSub]->position = cardSub;
     	sub[cardSub++]->parent = this;
	literals += other->sub[i]->literals;
    }

    // changing the formula makes old magic number obsolete - get new!
    if(cardSub != oldCard) // formula changed and not trivial
	{
	    magicnumber = MagicNumber::assign();
	}
}

arrayindex_t AtomicBooleanPredicate::getUpSet(arrayindex_t *stack, bool *onstack,
        bool *needEnabled) const
{
    if(isAnd)
    {
	// conjunction
        return sub[cardSat]->getUpSet(stack,onstack,needEnabled);
    }
    // disjunction
    arrayindex_t stackpointer = 0;
    for (arrayindex_t i = 0; i < cardSub; i++)
    {
        bool needEn = false;
        stackpointer += sub[i]->getUpSet(stack + stackpointer, onstack, &needEn);
        *needEnabled = *needEnabled || needEn;
        assert(stackpointer <= Net::Card[TR]);
    }
    return stackpointer;
}

/*!
If value of this changes, the parent formula is triggered for updating. This
means that updating is started at the leafs of the formula tree.  Parts of the
formula that did not change are not examined.

\param i  position of this formula in the parent's subformula list
*/
void AtomicBooleanPredicate::updateTF(arrayindex_t i)
{
 // assumption: satisfied left, unsatisfied right
 // --> sub[cardSat] is first unsatisfied

    if ((isAnd && (cardSat == cardSub)) || ((!isAnd) && (cardSat == 1)))
    {
	value = false;
	if (parent)
	{
	    parent->updateTF(position);
	}
    }
    cardSat--;
    StatePredicate *tmp = sub[cardSat];
    sub[cardSat] = sub[i];
    sub[i] = tmp;
    sub[i]->position = i;
    sub[cardSat]->position = cardSat;
}

/*!
If value of this changes, the parent formula is triggered for updating. This
means that updating is started at the leafs of the formula tree. Parts of the
formula that did not change are not examined.

\param i  position of this formula in the parent's subformula list
*/
void AtomicBooleanPredicate::updateFT(arrayindex_t i)
{
    // assumption: satisfied left, unsatisfied right

    // --> sub[cardSat] is first satisfied
    StatePredicate *tmp = sub[cardSat];
    sub[cardSat] = sub[i];
    sub[i] = tmp;
    sub[i]->position = i;
    sub[cardSat]->position = cardSat;
    ++cardSat;

    if ((isAnd && (cardSat == cardSub)) || ((!isAnd) && (cardSat == 1)))
    {
        value = true;
        if (parent)
        {
            parent->updateFT(position);
        }
    }
}



/*!
Evaluation starts top/down, so the whole formula is examined. Evaluation is
done w.r.t. Marking::Current.

\param ns  net state to evaluate the formula
*/
void AtomicBooleanPredicate::evaluate(NetState &ns)
{
    for (arrayindex_t i = 0; i < cardSub; i++)
    {
        sub[i]->evaluate(ns);
    }
    arrayindex_t left = 0;
    arrayindex_t right = cardSub;

    // sort satisfied to left, unsat to right of sub list
    // loop invariant: formulas left of left (not including left) are satisfied,
    // formulas right of right (including right) are unsatisfied
    while (true)
    {
        while (left < cardSub && sub[left]->value)
        {
            ++left;
        }
        while (right > 0 && !sub[right - 1]->value)
        {
            --right;
        }
        if (left >= right) // array sorted
        {
            break;
        }
        assert(left < cardSub);
        assert(right > 0);
        assert(right <= cardSub);
        StatePredicate *tmp = sub[left];
        sub[left++] = sub[--right];
        sub[right] = tmp;
    }
    cardSat = left;

    if(isAnd)
    {
	value = (cardSat == cardSub);
    }
    else
    {
	    value = (cardSat > 0);
    }

    // update position in sub formulas
    for (arrayindex_t i = 0; i < cardSub; i++)
    {
        sub[i]->position = i;
    }
}

/*!
Evaluation with Omega starts top/down, so the whole formula is examined. Evaluation is
done w.r.t. Marking::Current.

\param ns  net state to evaluate the formula including omega values
*/
void AtomicBooleanPredicate::evaluateOmega(NetState &ns)
{
    if(isAnd)
    {
	    bool true_unknown(false);
	    arrayindex_t false_unknown(0);
	    for (arrayindex_t i = 0; i < cardSub; i++)
	    {
		sub[i]->evaluateOmega(ns);
		if (sub[i]->unknown)
		{
		    if (sub[i]->value)
		    {
			true_unknown = true;
		    }
		    else
		    {
			++false_unknown;
		    }
		}
	    }
	    arrayindex_t left = 0;
	    arrayindex_t right = cardSub;
	    // sort satisfied to left, unsat to right of sub list
	    // loop invariant: formulas left of left (not including left) are satisfied,
	    // formulas right of right (including right) are unsatisfied/unknown
	    while (true)
	    {
		while (left < cardSub && sub[left]->value)
		{
		    ++left;
		}
		while (right > 0 && !sub[right - 1]->value)
		{
		    --right;
		}
		if (left >= right) // array sorted
		{
		    break;
		}
		assert(left < cardSub);
		assert(right > 0);
		assert(right <= cardSub);
		StatePredicate *tmp = sub[left];
		sub[left++] = sub[--right];
		sub[right] = tmp;
	    }
	    cardSat = left;

	    value = (cardSat == cardSub);
	    unknown = false;
	    if (value && true_unknown)
	    {
		unknown = true;
	    }
	    if (!value && cardSat + false_unknown == cardSub)
	    {
		unknown = true;
	    }

	    // update position in sub formulas
	    for (arrayindex_t i = 0; i < cardSub; i++)
	    {
		sub[i]->position = i;
	    }

    }
    else
    {
	    bool false_unknown(false);
	    arrayindex_t true_unknown(0);
	    for (arrayindex_t i = 0; i < cardSub; i++)
	    {
		sub[i]->evaluateOmega(ns);
		if (sub[i]->unknown)
		{
		    if (sub[i]->value)
		    {
			++true_unknown;
		    }
		    else
		    {
			false_unknown = true;
		    }
		}
	    }
	    arrayindex_t left = 0;
	    arrayindex_t right = cardSub;

	    // sort satisfied to left, unsat to right of sub list
	    // loop invariant: formulas left of left (not including left) are satisfied,
	    // formulas right of right (including right) are unsatisfied
	    while (true)
	    {
		while (left < cardSub && sub[left]->value)
		{
		    ++left;
		}
		while (right > 0 && !sub[right - 1]->value)
		{
		    --right;
		}
		if (left >= right) // array sorted
		{
		    break;
		}
		assert(left < cardSub);
		assert(right > 0);
		assert(right <= cardSub);
		StatePredicate *tmp = sub[left];
		sub[left++] = sub[--right];
		sub[right] = tmp;
	    }
	    cardSat = left;

	    value = (cardSat > 0);
	    unknown = false;
	    if (!value && false_unknown)
	    {
		unknown = true;
	    }
	    if (value && cardSat == true_unknown)
	    {
		unknown = true;
	    }

	    // update position in sub formulas
	    for (arrayindex_t i = 0; i < cardSub; i++)
	    {
		sub[i]->position = i;
	    }
    }
}

arrayindex_t AtomicBooleanPredicate::countAtomic() const
{
    arrayindex_t result = 0;

    for (arrayindex_t i = 0; i < cardSub; i++)
    {
        result += sub[i]->countAtomic();
    }
    return result;
}

arrayindex_t AtomicBooleanPredicate::collectAtomic(AtomicStatePredicate **p)
{
    arrayindex_t offset = 0;
    for (arrayindex_t i = 0; i < cardSub; i++)
    {
        offset += sub[i]->collectAtomic(p + offset);
    }
    return offset;
}

arrayindex_t AtomicBooleanPredicate::countDeadlock() const
{
    arrayindex_t result = 0;

    for (arrayindex_t i = 0; i < cardSub; i++)
    {
        result += sub[i]->countDeadlock();
    }
    return result;
}

arrayindex_t AtomicBooleanPredicate::countFireable() const
{
    arrayindex_t result = 0;

    for (arrayindex_t i = 0; i < cardSub; i++)
    {
        result += sub[i]->countFireable();
    }
    return result;
}

arrayindex_t AtomicBooleanPredicate::collectDeadlock(DeadlockPredicate **p)
{
    arrayindex_t offset = 0;
    for (arrayindex_t i = 0; i < cardSub; i++)
    {
        offset += sub[i]->collectDeadlock(p + offset);
    }
    return offset;
}

arrayindex_t AtomicBooleanPredicate::collectFireable(FireablePredicate **p)
{
    arrayindex_t offset = 0;
    for (arrayindex_t i = 0; i < cardSub; i++)
    {
        offset += sub[i]->collectFireable(p + offset);
    }
    return offset;
}

// LCOV_EXCL_START
bool AtomicBooleanPredicate::DEBUG__consistency(NetState &ns)
{
    for (arrayindex_t i = 0; i < cardSub; i++)
    {
        assert(sub[i]->DEBUG__consistency(ns));
        assert(sub[i]->position == i);
        assert(sub[i]->parent == this);
        assert(sub[i] != this);
        for (arrayindex_t j = 0; j < cardSub; j++)
        {
            if (i != j)
            {
                assert(sub[i] != sub[j]);
            }
        }
    }
    assert(cardSat <= cardSub);
    /*if (this != top)
    {
        assert(parent);
    }*/
    return true;
}

// LCOV_EXCL_STOP

/*!
\param parent  the parent predicate for the new, copied, object
*/
StatePredicate *AtomicBooleanPredicate::copy(StatePredicate *pt)
{
    AtomicBooleanPredicate *dsp = new AtomicBooleanPredicate(isAnd);
    dsp->magicnumber = magicnumber; // copy is equivalent to this
    dsp->cardSub = cardSub;
    dsp->cardSat = cardSat;
    dsp->literals = literals;
    dsp->value = value;
    dsp->position = position;
    dsp->parent = pt;
    // copy all sub-predicates, and give them the _new_ disjunction as parent
    dsp->sub = reinterpret_cast<StatePredicate **>(malloc(SIZEOF_VOIDP*(cardSub)));
    for (arrayindex_t i = 0; i < cardSub; i++)
    {
        dsp->sub[i] = sub[i]->copy(dsp);
	dsp->sub[i]->position = i;
    }
    return dsp;
}



arrayindex_t AtomicBooleanPredicate::getSubs(const StatePredicate *const **subs) const
{
    *subs = sub;
    return cardSub;
}

bool AtomicBooleanPredicate::isOrNode() const
{
    return !isAnd;
}

FormulaInfo *AtomicBooleanPredicate::getInfo() const
{
    FormulaInfo *Info = new FormulaInfo();

    if(isAnd)
    {
	Info->tag = formula_and;
    }
    else
    {
	    Info->tag = formula_or;
    }
    Info->cardChildren = cardSub;
    Info->statePredicateChildren = new StatePredicate * [cardSub];
    for (arrayindex_t i = 0; i < cardSub; i++)
    {
        Info->statePredicateChildren[i] = sub[i];
    }
    return Info;
}

int AtomicBooleanPredicate::countSubFormulas() const
{
    int sum = 1; // 1 for the root node
    for (arrayindex_t i = 0; i < cardSub; i++)
    {
        sum += sub[i]->countSubFormulas();
    }
    return sum;
}


char * AtomicBooleanPredicate::toString()
{
	int size = 50;
	char * result = (char *) malloc(size * sizeof(char));
	char * subresult;
	result[0] = '(';
	result[1] = '\0';

	for(int i = 0; i < cardSub; i++)
	{
		if(i!=0)
		{
			size += 5;
                        // Use a tmp pointer to avoid memleakOnRealloc
                        char * resultTmp = (char *) realloc(result, size * sizeof(char));
                        if (resultTmp == NULL)
                        {
                                // Could not realloc - free and exit
                                free(result);
                                RT::rep->status("realloc failed");
                                RT::rep->abort(ERROR_MEMORY);
                        }
                        result = resultTmp;
			if(isAnd)
                        {
				sprintf(result+strlen(result)," AND ");
			}
			else
			{
				sprintf(result+strlen(result)," OR  ");
			}
		}
		subresult = sub[i] -> toString();
		size += strlen(subresult);
		// Use a tmp pointer to avoid memleakOnRealloc
		char * resultTmp = (char *) realloc(result, size * sizeof(char));
		if (resultTmp == NULL)
		{
			// Could not realloc - free and exit
			free(result);
			RT::rep->status("realloc failed");
                        RT::rep->abort(ERROR_MEMORY);
		}
		result = resultTmp;
		sprintf(result+strlen(result),"%s",subresult);
		free(subresult);
	}
	sprintf(result+strlen(result),")");
	return result;
}

StatePredicate * AtomicBooleanPredicate::negate()
{
	magicnumber = -magicnumber; // remember effect of negation in 
				   // magic number
	isAnd = !isAnd;
	for(arrayindex_t i = 0; i < cardSub; i++)
	{
		sub[i] = sub[i]->negate();
		sub[i]->parent = this;
		sub[i]->position = i;
	}
	return this;
}

void AtomicBooleanPredicate::adjust(arrayindex_t old,arrayindex_t nw)
{
	for(arrayindex_t i = 0; i < cardSub; i++)
	{
		sub[i]->adjust(old,nw);
	}
}

void AtomicBooleanPredicate::setVisible()
{
	for(arrayindex_t i = 0; i < cardSub; i++)
	{
		sub[i]->setVisible();
	}
}

AtomicBooleanPredicate * AtomicBooleanPredicate::DNF()
{
	if(cardSub == 0)
	{
		return this; // empty conj or empty disj 
	}
	if(isAnd)
	{
		// our subformulas are &ed
		AtomicBooleanPredicate * result = NULL; // create on demand
		for(arrayindex_t i = 0; i < cardSub; i++)
		{
			AtomicBooleanPredicate * subresult = sub[i]->DNF();
			if(!subresult)
			{
				// subformula illegal or too big
				return NULL;
			}
			// handle contradiction/tautology in subresult
			if(subresult -> magicnumber == MAGIC_NUMBER_FALSE)
			{
				// subresult is contradiction
				result = new AtomicBooleanPredicate(true);
				result -> magicnumber = MAGIC_NUMBER_FALSE;
				return result;
			}
			if(subresult -> magicnumber == MAGIC_NUMBER_TRUE)
			{
				// subresult does not contribute
				continue;
			}
			if(!result)
			{
				result = subresult;
				continue;
			}
			if(subresult->isAnd)
			{
				//  ??? & conj
				if(!(result->isAnd))
				{
					// disj & conj
					// need to apply law of distrib
					if(result->literals+result->cardSub*subresult->literals > MAX_DNF_LENGTH)
					{
						return NULL;
					}
					AtomicBooleanPredicate * newresult = new AtomicBooleanPredicate(false);
					for(arrayindex_t i = 0; i < result->cardSub;i++)
					{
						AtomicBooleanPredicate * tmp1 = ((AtomicBooleanPredicate*)(subresult->copy(NULL)));
				
						AtomicBooleanPredicate * tmp2 = ((AtomicBooleanPredicate*)(result->sub[i]));
						tmp2 ->merge(tmp1);
						newresult->addSub(tmp2);
					}
					result = newresult;
					if(result->magicnumber == MAGIC_NUMBER_TRUE) // contains sub and -sub
					{
						result = NULL;
					}
				}
				else
				{
					// conj & conj
					// -> merge result, subresult
					if(result->literals+subresult->literals > MAX_DNF_LENGTH)
					{
						return NULL;
					}
					// conj & conj
					// my_disj_all remains 1
					result->merge(subresult);

					if(result->magicnumber == MAGIC_NUMBER_FALSE) // contains sub and -sub
					{
						return result;
					}
				}
			}
			else
			{
				// subresult is disj

				if(!(result->isAnd))
				{
					// disj & disj
					// need to apply law of distrib
					// to result,subresult
					if(result->cardSub * subresult->literals + subresult->cardSub * result->literals > MAX_DNF_LENGTH)
					{
						return NULL;
					}
					AtomicBooleanPredicate * newresult = new AtomicBooleanPredicate(false);
					for(arrayindex_t i = 0; i < result->cardSub;i++)
					{
						for(arrayindex_t j = 0; j < subresult -> cardSub; j++)
						{
							AtomicBooleanPredicate * tmp1 = ((AtomicBooleanPredicate*)(subresult->sub[j]->copy(NULL)));
							AtomicBooleanPredicate * tmp2 = ((AtomicBooleanPredicate*)(result->sub[i]->copy(NULL)));
							tmp2 ->merge(tmp1);
							newresult -> addSub(tmp2);
							if(newresult -> magicnumber == MAGIC_NUMBER_TRUE) break; // cannot become trueer
						}
						if(newresult -> magicnumber == MAGIC_NUMBER_TRUE) break; // cannot become trueer
					}
					result = newresult;
					if(result->magicnumber == MAGIC_NUMBER_TRUE) // contains sub and -sub
					{
						result = NULL;
					}
					if(result->magicnumber == MAGIC_NUMBER_FALSE) // contains sub and -sub
					{
						return result;
					}
				}
				else
				{
					// conj & disj
					if(subresult->literals + subresult->cardSub*result->literals > MAX_DNF_LENGTH)
					{
						return NULL;
					}
					// apply law of distr
					AtomicBooleanPredicate * newresult = new AtomicBooleanPredicate(false);
					for(arrayindex_t j = 0; j < subresult -> cardSub; j++)
					{
						AtomicBooleanPredicate * tmp1 = ((AtomicBooleanPredicate*)(subresult->sub[j]));
						AtomicBooleanPredicate * tmp2 = ((AtomicBooleanPredicate*)(result->copy(NULL)));
						tmp1 ->merge(tmp2);
						newresult ->addSub(tmp1);
					}
					result = newresult;
					if(result->magicnumber == MAGIC_NUMBER_TRUE) 
					{
						result = NULL;
					}
					if(result->magicnumber == MAGIC_NUMBER_FALSE)
					{
						return result;
					}
				}
			}
		}
		if(!result) result = new AtomicBooleanPredicate(true);
		return result;
	}
	else
	{
		AtomicBooleanPredicate * result = NULL; // create on demand

		// our subformulas are |ed
		for(arrayindex_t i = 0; i < cardSub; i++)
		{
			AtomicBooleanPredicate * subresult = sub[i]->DNF();
			if(!subresult)
			{
				// subformula illegal or too big
				return NULL;
			}
			if(subresult->magicnumber == MAGIC_NUMBER_TRUE)
			{
				return subresult; // x | true = true
			}
			if(subresult->magicnumber == MAGIC_NUMBER_FALSE)
			{
				continue; // x | false = x
			}
			if(!result)
			{
				result = subresult;
				continue;
			}
			if(subresult->isAnd)
			{
				// ??? | conj
				// adding a conjunction
				if(result->isAnd)
				{
					//conj | conj
					if(result->literals+subresult->literals > MAX_DNF_LENGTH)
					{
						// dnf gets too big
						return NULL;
					}
					AtomicBooleanPredicate * newresult = new AtomicBooleanPredicate(false); // build new disjunction
					newresult -> addSub(result);
					newresult -> addSub(subresult);
					if(newresult->magicnumber == MAGIC_NUMBER_TRUE) // contains sub and -sub
					{
						return newresult;
					}
					if(newresult->magicnumber == MAGIC_NUMBER_FALSE) // contains sub and -sub
					{
						result = NULL;
					}
					else
					{
						result = newresult;
					}
				}
				else
				{
					// disj | conj
					if(result -> literals + subresult->literals > MAX_DNF_LENGTH)
					{
						// dnf gets too big
						return NULL;
					}
					result->addSub(subresult);
					if(result->magicnumber == MAGIC_NUMBER_TRUE) // contains sub and -sub
					{
						return result;
					}
					if(result->magicnumber == MAGIC_NUMBER_FALSE)					     {
						result = NULL;
					}
				}
				
			}
			else
			{
				// subresult is disj
				// ??? | disj
				if(result->isAnd)
				{
					// conj | disj
					if(result->literals+subresult->literals > MAX_DNF_LENGTH)
					{
						// dnf gets too big
						return NULL;
					}
					subresult -> addSub(result);
					result = subresult;
					if(result->magicnumber == MAGIC_NUMBER_TRUE) // contains sub and -sub
					{
						return result;
					}
				}
				else
				{
					// disj | disj
					if(result->literals+subresult->literals > MAX_DNF_LENGTH)
					{
						// dnf gets too big
						return NULL;
					}
					result -> merge(subresult);

					if(result->magicnumber == MAGIC_NUMBER_TRUE) // contains sub and -sub
					{
						return result;
					}
				}
			}
		}
		if(!result) result = new AtomicBooleanPredicate(false);
		return result;
	}
}

int compareAtoms (StatePredicate * left, StatePredicate * right)
{
	// compare two state predicates. We assume
	// - both left and right are actually comparisons (AtomicStatePredicate)

	// result: -1: left < right, 0, left = right, 1: left > right

	// apply the following order
	// Priority 1: threshold
	// Priority 2: lex in 
	// 	2a: posPlaces
	// 	2b: posMult
	// Priority 3: lex in 
	//	3a: negPlaces
	//	3b: negMult

	AtomicStatePredicate * L = (AtomicStatePredicate *) left;
	AtomicStatePredicate * R = (AtomicStatePredicate *) right;
	
	if(L -> threshold < R -> threshold) return -1;
	if(R -> threshold < L -> threshold) return 1;
	for(arrayindex_t i = 0; (i < L ->cardPos) && (i < R -> cardPos); i++)
	{
		if(L -> posPlaces[i] < R -> posPlaces[i]) return -1;
		if(R -> posPlaces[i] < L -> posPlaces[i]) return 1;
		if(L -> posMult[i] < R -> posMult[i]) return -1;
		if(R -> posMult[i] < L -> posMult[i]) return 1;
	}
	if(L -> cardPos < R -> cardPos) return -1;
	if(R -> cardPos < L -> cardPos) return 1;
	for(arrayindex_t i = 0; (i < L -> cardNeg) && (i < R -> cardNeg); i++)
	{
		if(L -> negPlaces[i] < R -> negPlaces[i]) return -1;
		if(R -> negPlaces[i] < L -> negPlaces[i]) return 1;
		if(L -> negMult[i] < R -> negMult[i]) return -1;
		if(R -> negMult[i] < L -> negMult[i]) return 1;
	}
	if(L -> cardNeg < R -> cardNeg) return -1;
	if(R -> cardNeg < L -> cardNeg) return 1;
	return 0;
}

void AtomicBooleanPredicate::addSubSorted(StatePredicate *f)
{
    if(sub)
    {
	int c = 0;
	arrayindex_t i;
	for(i = 0; i < cardSub;i++)
	{
		c = compareAtoms(sub[i],f);
		if(c == 0) return;
		if(c > 0) break;
	}
        sub = reinterpret_cast<StatePredicate **>(realloc(sub,SIZEOF_VOIDP * (cardSub + 1)));
	for(arrayindex_t j = cardSub; j>i ;j--)
	{
		sub[j] = sub[j-1];
		sub[j]->position = j;
	}
	sub[i] = f;
        f->position = i;
        f->parent = this;
        cardSub++;
    }
    else
    {
        sub = reinterpret_cast<StatePredicate **>(malloc(SIZEOF_VOIDP*(cardSub+1)));
    	f->position = cardSub;
    	f->parent = this;
    	sub[cardSub++] = f;
    }
    // formula changed
    magicnumber = MagicNumber::assign();
}

void AtomicBooleanPredicate::mergeSorted(AtomicBooleanPredicate * other)
{
    magicnumber = MagicNumber::assign(); // get new magic number
    StatePredicate ** resultsub = reinterpret_cast<StatePredicate **>(malloc(SIZEOF_VOIDP * (cardSub+other->cardSub)));

    arrayindex_t i = 0;
    arrayindex_t j = 0;
    arrayindex_t k = 0;

    while((i < cardSub) && (j < other -> cardSub))
    {
	int c = compareAtoms(sub[i],other->sub[j]);
 	if(c == 0)
	{
		j++; // have found duplicate
		continue;
	}
	if(c < 0)
	{
		resultsub[k] = sub[i];
		sub[i]->position = k;
		i++;
		k++;
		continue;
	}
	//(c > 0)
	{
		resultsub[k] = other->sub[j];
		resultsub[k] -> position = k;
		resultsub[k] -> parent = this;
		j++;
		k++;
	}
    }
    while(i < cardSub)
    {
	resultsub[k] = sub[i];
	sub[i]->position = k;
	i++;
	k++;
	continue;
    }
    while(j < other->cardSub)
    {
	resultsub[k] = other->sub[j];
	resultsub[k] -> position = k;
	resultsub[k] -> parent = this;
	j++;
	k++;

    }
    if(sub) free(sub);
    sub = resultsub;
}

FormulaStatistics * AtomicBooleanPredicate::count(FormulaStatistics * fs)
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
	for(arrayindex_t i = 0; i < cardSub; i++)
	{
		fs = sub[i]->count(fs);
	}
	if(isAnd)
	{
		fs->aconj++;
	}
	else
	{
		fs->adisj++;
	}
	return fs;
}

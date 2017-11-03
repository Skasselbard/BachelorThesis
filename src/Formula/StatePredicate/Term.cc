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

\brief temporal list based container for terms in atomic propositions

*/

#include<Formula/StatePredicate/Term.h>
#include<Core/Dimensions.h>
#include<CoverGraph/CoverGraph.h>
#include<Net/Net.h>

Term::Term()
{
	card = 1;
	next = NULL;
	place = 0;
	mult = 0;
}

Term::~Term()
{
	delete next;
}

void Term::multiply(int32_t f)
{
	if(f == 1)
	{
		// do nothing
		return;
	}
	if(mult == OMEGA)
	{
		if(f <= -1)
		{
			mult = -OMEGA;
		}
		else if (f >= 1)
		{
			mult = OMEGA;
		}
		else // f == 0
		{
		     RT::rep->message("%s: multiplication 0 * oo",
              RT::rep->markup(MARKUP_WARNING, "error").str());
		RT::rep->abort(ERROR_SYNTAX);

		}
	}
	else if(mult == -OMEGA)
	{
		if(f <= -1)
		{
			mult = OMEGA;
		}
		else if (f >= 1)
		{
			mult = -OMEGA;
		}
		else // f == 0
		{
		     RT::rep->message("%s: multiplication 0 * oo",
              RT::rep->markup(MARKUP_WARNING, "error").str());
		RT::rep->abort(ERROR_SYNTAX);

		}
	}
	else
	{
		mult *= f;
	}
	if(next)
	{
		next -> multiply(f);
	}
}

void Term::append(Term * other)
{
	if(next)
	{
		next -> append(other);
	}
	else
	{
		next = other;
	}
	card += other -> card;
}

Term * Term::copy()
{
	Term * T = new Term();
	T -> place = place;
	T -> mult = mult;
	if(next)
	{
		T -> next = next -> copy();
	}
	return T;
}

void Term::print()
{
	
	if(place == Net::Card[PL])
	{
		RT:: rep -> status("%d + ",mult);
	}
	else
	{
		RT::rep->status("%d * %s + ",mult,Net::Name[PL][place]);
	}
	if(next)
	{
		next -> print(); 
	}
	else
	{
		RT::rep->status(".");
	}
}

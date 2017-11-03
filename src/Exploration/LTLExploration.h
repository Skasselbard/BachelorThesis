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

\brief Evaluates an LTL Property
*/

#pragma once

#include <config.h>
#include <Exploration/SearchStack.h>
#include <Stores/Store.h>

class BuechiAutomata;
class Path;
class Firelist;
class NetState;
class LTLSearchStackEntry;

class LTLPayload
{
	public:
	LTLPayload();
	~LTLPayload();
	uint64_t dfsnum;
	uint64_t lowlink;
	LTLPayload* witnessstate; // the state defining this lowlink
	arrayindex_t witness; // the transition that leads to the state
			      // where this state gats its lowlink from
};

class LTLExploration // In this version, we do not care about fairness!!!!!!!
{
public:
    explicit LTLExploration();

    bool checkProperty(BuechiAutomata &automata, Store<LTLPayload> &store,
                       Firelist &firelist, NetState &ns);

    /// return a witness path
    Path * path;
void createWitness(LTLPayload * , SearchStack<LTLSearchStackEntry> *);
};

/// An entry in the LTL search stack
class LTLSearchStackEntry
{
	public:
        /// the firelist of stacked state
	arrayindex_t * fl;
	/// the most recently processed entry in firelist
	arrayindex_t netIndex;
	/// the buchi automaton state of stacked state
	arrayindex_t buchiState;
	/// the list of enabled buchi transitions
	arrayindex_t * bl;
	/// the length of bl
	arrayindex_t  buchiLength;
	/// the most recently processed transition in bl 
	arrayindex_t buchiIndex;
	/// link to state
	LTLPayload * info;
	LTLSearchStackEntry(arrayindex_t * f,arrayindex_t ni,arrayindex_t bs,arrayindex_t * bbl, arrayindex_t l, arrayindex_t bi, LTLPayload * inf)
	{
		fl = f;
		netIndex = ni;
		buchiState = bs;
		bl = bbl;
		buchiLength = l;
		buchiIndex = bi;
		info = inf;
	}
	~LTLSearchStackEntry()
	{
		delete fl;
		delete bl;
	}
};


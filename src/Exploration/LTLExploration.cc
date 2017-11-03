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

#include <Exploration/Firelist.h>
#include <Exploration/LTLExploration.h>
#include <Exploration/LTLStack.h>
#include <Formula/LTL/BuechiAutomata.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Marking.h>
#include <Net/NetState.h>
#include <Net/Transition.h>
#include <Witness/Path.h>


LTLPayload::LTLPayload()
{}
LTLPayload::~LTLPayload()
{}


LTLExploration::LTLExploration() 
{}


bool LTLExploration::checkProperty(BuechiAutomata &automaton,
                                   Store<LTLPayload> &store, Firelist &firelist, NetState &ns)
{
    LTLPayload * stateinfo;

     path = new Path();
    /// State of Buchi automaton
    arrayindex_t buchistate = 0;

    // In general, buchistate is part of currentmarking, at index
    // Place::CardSignificant -1. For the initial marking, 0 is
    // already set by function insertbuchistate() in Planning/LTLTask.cc

    // Initial state gets dfs number 1. Value 0 is reserved for states
    // that have left the tarjan stack (are member of completed SCC).
    store.searchAndInsert(ns, &stateinfo, 0);
    stateinfo->dfsnum = 1;
    stateinfo->lowlink = 1;
    stateinfo->witness = Net::Card[TR]; // this state has its lowlink from itself
 stateinfo->witnessstate = NULL;
    /// current global dfs number
    uint64_t nextDFSNumber = 2;

    // Initialize stacks. We work with 3 stacks:
    // * DFSStack: for organizing depth first search, carries firelist
    //   Information.
    // * TarjanStack: for states that are fully processed but their SCC
    //   has not yet been completed. Carries pointer to payload. 
    //   State is pushed when first visited and popped when its SCC is
    //   complete.
    //   When state is pushed, payload of state is set to [0,0].
    // * AcceptingStack: for managing the accepting state with largest
    //   dfs on DFSStack. Carries dfsnums. State is pushed when 
    //   visited first, and popped when it leaves the DFSStack.

    SearchStack<LTLSearchStackEntry> * DFSStack = new SearchStack<LTLSearchStackEntry>;
    SearchStack<LTLPayload *> TarjanStack;
    SearchStack<arrayindex_t> AcceptingStack;

    // initialize AcceptingStack with a dummy entry containing the
    // unused dfs number 0. This way, we can refer to top of stack
    // even if no accepting states have been seen so far...

    arrayindex_t * acc = AcceptingStack.push(); 
    * acc = 0; // set stack content

    if(automaton.isAcceptingState(buchistate))
    {
	// the first "real" entry on AcceptingStack
	acc = AcceptingStack.push();
        * acc = 1;
    }

    // Initialize TarjanStack with stateinfo
    LTLPayload ** ppp = TarjanStack.push();
    * ppp = stateinfo;

    // get first firelist
    // note that we process firelist and automaton transition list from
    // card to 0. This way, we do not need to store the size of the lists.

    arrayindex_t *currentFirelist;
    arrayindex_t currentEntry = firelist.getFirelist(ns, &currentFirelist);
    if(currentEntry == 0)
    {
	// Initial state is deadlock state. We need to continue since
        // LTL semantics turns deadlock into tau transition.
 	currentFirelist = new arrayindex_t[1]; 
	currentFirelist[0] = Net::Card[TR]; // our code for tau transition
	currentEntry = 1;
    }

    // get first transition list in automaton
    arrayindex_t * currentBuchiList = reinterpret_cast<arrayindex_t *>(malloc(SIZEOF_ARRAYINDEX_T * automaton.cardTransitions[buchistate]));
    arrayindex_t currentBuchiTransition = 0;
    for(arrayindex_t i = 0; i < automaton.cardTransitions[buchistate];i++)
    {
	StatePredicate * predicate = automaton.guard[buchistate][i];
        predicate -> evaluate(ns);
	if(predicate->value)
	{
		currentBuchiList[currentBuchiTransition++] = automaton.nextstate[buchistate][i];
	}
    }
    arrayindex_t buchiLength = currentBuchiTransition;
    if(!buchiLength)
    {
	// situation: no buchi transition enabled in initial state->
	// cannot accept anything
	return false;
    }
    currentBuchiList = reinterpret_cast<arrayindex_t *>(realloc(currentBuchiList,SIZEOF_ARRAYINDEX_T * buchiLength));
    // Initialize DFSStack with ingredients of initial state
    LTLSearchStackEntry * stackentry = DFSStack->push();
    stackentry  = new (stackentry) LTLSearchStackEntry(currentFirelist,currentEntry,buchistate,currentBuchiList,buchiLength,currentBuchiTransition,stateinfo);

    // For the stubborn set method, we need to avoid ignorance. We do so by
    // taking care that we fire, at least once in every TSCC, all enabled
    // transitions. Two variables control this:
    // - lastTSCC: dfs number of root of most recently detected tscc
    // - lastFiredAll: highest dfs number where all enabled transitions fired
    // For distinguishing tscc from other scc, we use the following observation:
    // * the first detected scc is a tscc
    // * every non-terminal scc has a root with dfs smaller than a previously
    //   detected tscc
    // * every tscc has root that has dfs larger than any root of any
    //   previously detected scc
    // --> scc is tscc if and only if its root dfs is greater than lastTSCC
    // Upon detection of a tscc, lastFiredAll >= dfs(root) means that
    // at least once in the tscc, all transitions have fired, i.e. no
    // transitions are ignored. Otherwise, we extend firelist at root to
    // all enabled transitions.

    arrayindex_t lastTSCC = 0; // no tscc detected yet
    arrayindex_t lastFiredAll = 0; // never fired all enabled transitions
    if(currentEntry == ns.CardEnabled) lastFiredAll = 1; // fired all in initial marking

     while(true) // DFS search loop
     {
	// situation: we have a state, given by ns and buchistate.
        // We have a firelist with an entry and a buchi transition.
	// task: continue processing this state.
       
        if(currentEntry)
	{
		// situation: our state has enabled net transitions (or tau
		// transition) that
		// have not been fully explored. The one to be considered
 		// is currentEntry - 1.
		// task: continue processing this net transition by
		// joining it with next enabled buchi transition
//Marking::DEBUG__printMarking(ns.Current);
		//RT::rep->status("has enabled net transition");

		if(currentBuchiTransition)
		{
			// situation: There is an enabled buchi transition 
			// not yet explored (currentBuchiTransition-1)
			// task: explore this buchi transition together
			// with current net transition
			//RT::rep->status("has enabled buchi transition");
			

			if(currentFirelist[currentEntry-1] >= Net::Card[TR])
			{
				// fire tau transition = do nothing
				//RT::rep->status("fire tau");
			}
			else
			{
				Transition::fire(ns, currentFirelist[currentEntry-1]);
				//RT::rep->status("fire %s",Net::Name[TR][currentFirelist[currentEntry-1]]);
			}
			ns.Current[Place::CardSignificant-1] = currentBuchiList[--currentBuchiTransition];

			// new product state ready for search

			LTLPayload * newstateinfo;
			if(store.searchAndInsert(ns,&newstateinfo,0))
			{
				// situation: new state exists
				// task: update lowlink, check acceptance,
				// continue at current state
				//RT::rep->status("new state exists dfs %u low %u",newstateinfo->dfsnum,newstateinfo->lowlink);

				if(newstateinfo->dfsnum > 0)
				{
					// situation: new state still on
					// tarjan stack
					// task: update lowlink and
					// check acceptance
	//RT::rep->status("new state on tarjan");

					if(stateinfo->lowlink > newstateinfo->lowlink)
					{
						// situation: lowlink improves
						// task: update, check acceptance
	//RT::rep->status("lowlink improves");
						stateinfo->lowlink = newstateinfo->lowlink;
						stateinfo->witness = currentFirelist[currentEntry-1]; // this transition leads to the state defining its lowlink
						stateinfo->witnessstate = newstateinfo;

						// acceptance check
						if(stateinfo->lowlink <= AcceptingStack.top())
						{
							// situation: ACCEPT
							// task: tidy up and
							// return true
//RT::rep->status("accept");
	
							createWitness(newstateinfo, DFSStack);
							return true;
						}
						// situation: not accepted
						// task: continue at current
	//RT::rep->status("do not accept");

						if(currentFirelist[currentEntry-1] < Net::Card[TR])
						{
							Transition::backfire(ns, currentFirelist[currentEntry-1]);
						}
						continue;
					}
					else
					{
						// situation: lowlink not improved
						// task: check accepting self-loop
						if((stateinfo->dfsnum == newstateinfo->dfsnum) && (stateinfo->dfsnum == AcceptingStack.top()))
						{
							// situation: have accepting self-loop
							// task: accpet
							createWitness(newstateinfo, DFSStack);
							return true;
						}
						// situation: lowlink not improved, no accepting self-loop
						// task: no accept, continue
	//RT::rep->status("lowlink not improved");
						if(currentFirelist[currentEntry-1] < Net::Card[TR])
						{
							Transition::backfire(ns, currentFirelist[currentEntry-1]);
						}
						continue;
					}
					// end of complete if-else
					assert(false);
				}
				else
				{
					// situation: new state not on 
					// tarjan stack
					// task: no lowlink update, no
					// acceptance check necessary
					// continue at current state
	//RT::rep->status("new state not on trajan");

					if(currentFirelist[currentEntry-1] < Net::Card[TR])
					{
						Transition::backfire(ns, currentFirelist[currentEntry-1]);
					}
					continue;
				}
				// end of complete if-else
				assert(false);
			
			}
			else
			{
				// situation: new state does not yet exist
				// task: fully switch to new state and
				// continue search at new state
	//RT::rep->status("new state does not yet exist");

				// update stack entry of current state
				stackentry -> netIndex = currentEntry;
				stackentry -> buchiIndex = currentBuchiTransition;
				
				// initialize data for new state
				
				// dfsnum + lowlink
				stateinfo = newstateinfo;
				stateinfo->dfsnum = stateinfo->lowlink = nextDFSNumber++;
				stateinfo->witness = Net::Card[TR];
				stateinfo->witnessstate = NULL;

				// state of buchi automaton
				buchistate = currentBuchiList[currentBuchiTransition];
				// acceptance stack
				if(automaton.isAcceptingState(buchistate))
				{
					acc = AcceptingStack.push();
					* acc = stateinfo->dfsnum;
				}

				// tarjan stack
			        ppp = TarjanStack.push();
			        * ppp = stateinfo;

				// net firelist

	//RT::rep->status(" 1 new state does not yet exist %u %u",currentEntry,currentFirelist[currentEntry - 1]);
				if(currentFirelist[currentEntry - 1] < Net::Card[TR])
				{
					// net transition was not tau
					Transition::updateEnabled(ns, currentFirelist[currentEntry-1]);
				}
	//RT::rep->status(" 2 new state does not yet exist");
				currentEntry = firelist.getFirelist(ns, &currentFirelist);
				if(currentEntry >= ns.CardEnabled)
				{
					lastFiredAll = stateinfo->dfsnum;
				}
				if(currentEntry == 0)
				{
					// State is deadlock state. We need to continue since
					// LTL semantics turns deadlock into tau transition.
					currentFirelist = new arrayindex_t[1]; 
					currentFirelist[0] = Net::Card[TR]; // our code for tau transition
					currentEntry = 1;
				}
	
				// buchi firelist
				currentBuchiList = reinterpret_cast<arrayindex_t *>(malloc(SIZEOF_ARRAYINDEX_T * automaton.cardTransitions[buchistate]));
				currentBuchiTransition = 0;
				for(arrayindex_t i = 0; i < automaton.cardTransitions[buchistate];i++)
				{
					StatePredicate * predicate = automaton.guard[buchistate][i];
					predicate -> evaluate(ns);
					if(predicate->value)
					{
						currentBuchiList[currentBuchiTransition++] = automaton.nextstate[buchistate][i];
					}
				}
				buchiLength = currentBuchiTransition;
				if(!buchiLength)
				{
//RT::rep->status("empty buchi");
					// situation: no enabled buchi
					// transition, i.e. no successor in
					// product automaton
					// task: shortcut: set net fl to
					// completed
					currentBuchiList = NULL;
					currentEntry = 0;
				}
				else
				{
					currentBuchiList = reinterpret_cast<arrayindex_t *>(realloc(currentBuchiList,SIZEOF_ARRAYINDEX_T * buchiLength));
				}
				
	//RT::rep->status(" 3 new state does not yet exist");
				// search stack
				stackentry = DFSStack->push();
				stackentry  = new (stackentry) LTLSearchStackEntry(currentFirelist,currentEntry,buchistate,currentBuchiList,buchiLength,currentBuchiTransition,stateinfo);
				continue;
			}
			// end of complete if-else
			assert(false);
		}
		else
		{
			// situation: all buchi transitions have been
			// explored.
			// task: proceed with next net transition, 
			
//RT::rep->status("all buchi transitions explored");
			--currentEntry;
			currentBuchiTransition = buchiLength;
			continue;
		}
		// end of complete if-else
		assert(false);
	}
	else
	{
		// situation: all enabled transitions of this state have
		// been processed.
		// task: close this state (check termination, check scc, backtrack)
	//RT::rep->status("closing");

		if(stateinfo->dfsnum == 1)
		{
			// situation: we are closing initial state
			// task: check ignorance
	//RT::rep->status("closing initial");
			if(lastTSCC < 1)
			{
				// situation: initial state is root of TSCC
				// task: check ignorance

				if(lastFiredAll < 1)
				{
					// situation: have ignored 
					delete [] currentFirelist;
					currentFirelist = new arrayindex_t[ns.CardEnabled];
					currentEntry = 0;
					for(arrayindex_t z = 0; z < Net::Card[TR];z++)
					{
						if(ns.Enabled[z])
						{
							currentFirelist[currentEntry++] = z;
						}
					}
					stackentry -> netIndex = currentEntry;
  					stackentry -> fl = currentFirelist;
					lastFiredAll = 1;
					continue;
				}
				else
				{
					// situation: do not have ignored
					// task: exit
				
					return false;
				}
				// end of complete if/else
				assert(false);
			}
			else
			{
				// situation: initial state is not root of TSCC
				// task: no ignorance check necessary->exit

				return false;
			}
			// end of complete if/else
			assert(false);
		}
		// situation: we are closing any non-initial state
		// task: check scc, backtrack
		
		// keep lowlink for update of predecessor state
		arrayindex_t lowli = stateinfo->lowlink;
		if(stateinfo->lowlink == stateinfo->dfsnum)
		{
			// situation: scc found
			// check whether scc is tscc

			if(stateinfo->dfsnum > lastTSCC)
			{
				// situation: closing tscc
				// task: check ignored transitions
				if(lastFiredAll < stateinfo->dfsnum)
				{
	//RT::rep->status("adding ignored");
					// situation: have ignored
					// task: extend firelist
					delete [] currentFirelist;
					currentEntry = 0;
					currentFirelist = new arrayindex_t[ns.CardEnabled];
					for(arrayindex_t z = 0; z < Net::Card[TR];z++)
					{
						if(ns.Enabled[z])
						{
							currentFirelist[currentEntry++] = z;
						}
					}
					stackentry->fl = currentFirelist;
					stackentry->netIndex = currentEntry;
					lastFiredAll = stateinfo->dfsnum;
				}
				// situation: no ignored transitions
				// task: remove tscc from tarjan stack
				lastTSCC = stateinfo->dfsnum;
	
			}
			// situation: closing noterminal scc or
			// closing tscc without ignored transitions
			// task: remove scc from tarjan stack
	//RT::rep->status("closing scc");

			LTLPayload * pp;
			do
			{
				pp = TarjanStack.top();
				TarjanStack.pop();
				pp->dfsnum = pp->lowlink = 0;
			}
			while(pp!=stateinfo);
		}
		// situation: no scc found, or scc already removed, state is
		// not initial
		// task: backtrack
	//RT::rep->status("backtracking");

		// backtrack search stack
		DFSStack->pop();

		// load data for previous state from search stack
		stackentry = &(DFSStack->top());
		currentFirelist = stackentry->fl;
		currentEntry = stackentry->netIndex;
		buchistate = stackentry->buchiState;
		currentBuchiList = stackentry->bl;
		buchiLength = stackentry->buchiLength;
		currentBuchiTransition = stackentry->buchiIndex;
		LTLPayload * oldstateinfo = stateinfo;
		stateinfo = stackentry->info;

		// update lowlink
		if(stateinfo->lowlink > lowli)
		{
			stateinfo->lowlink = lowli;
			stateinfo->witnessstate = oldstateinfo;
			stateinfo->witness = currentFirelist[currentEntry-1];
		}

		// backtrack marking
		if(currentFirelist[currentEntry-1] < Net::Card[TR])
		{
			Transition::backfire(ns, currentFirelist[currentEntry-1]);
			Transition::revertEnabled(ns, currentFirelist[currentEntry-1]);
		}
		ns.Current[Place::CardSignificant-1] = buchistate;

		// backtrack acceptance stack
		while(AcceptingStack.top() > stateinfo->dfsnum)
		{
			AcceptingStack.pop();
		}
		continue;
	}
	// end of complete if-else
	assert(false);
     }
     // end of while loop, only keft by return true/return false
     assert(false);
}

void LTLExploration::createWitness(LTLPayload * stateinfo, SearchStack<LTLSearchStackEntry> * DFSStack)
{
	// situation: stateinfo belongs to a state s with a lowlink l less 
	// than some
	// accepting state on the search stack. Hence, the witness path
	// has shape a (b c)* where
	// - c starts at stateinfo (the top of search stack) and ends
	// at a state s* that is on the search stack and has dfsnum l
	// - a is the search stack from m0 to s*
	// - b is the search stack from s* to s
	// We need to start with computing s*
	LTLPayload * sstar;
	for(sstar = stateinfo; ;sstar = sstar -> witnessstate)
	{
		if(! sstar -> witnessstate )	break; // does not lead to 
						// smaller dfs
	}
	// now insert a b in reverse order (as we access stack from top to bottom)
	// all transitions are inserted in front of the others (add(...,true))
	// At the boundary between a and bm add "beginCycle"
	path = new Path();
 	path->initialized = true;
	bool ccc = false;
	while(DFSStack->StackPointer != 0)
	{
		LTLSearchStackEntry e = DFSStack->top();
		path->addTransition(e.fl[e.netIndex-1],true);
		if(e.info == sstar){ path->beginCycle(true);ccc=true;}
		DFSStack->pop();
	}
	if(!ccc) path->beginCycle();
	// now: add c at end of sequence
	for(;stateinfo!=sstar;stateinfo = stateinfo->witnessstate)
	{
		RT::rep->status("add low %u",stateinfo->witness);
		path->addTransition(stateinfo->witness);
	}
	path->endCycle();
}


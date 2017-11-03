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

\brief Class for firelist generation by the deletion algorithm for stubborn
sets.
*/

#include <Core/Dimensions.h>
#include <Exploration/Firelist.h>
#include <Exploration/FirelistStubbornCombinedDeadlock.h>
#include <Net/Net.h>
#include <Net/NetState.h>
#include <Net/Transition.h>
#include <Core/Runtime.h>

#ifndef INT32_MAX
#define INT32_MAX (2147483647)
#endif

/*!
 * \brief A constructor for firelists of stubborn sets using the deletion algorithm.
 *        Used for deadlock checks.
 */
FirelistStubbornCombinedDeadlock::FirelistStubbornCombinedDeadlock():
    dfsStack(new arrayindex_t[Net::Card[TR]]),
    dfs(new arrayindex_t[Net::Card[TR]]),
    lowlink(new arrayindex_t[Net::Card[TR]]),
    currentIndex(new arrayindex_t[Net::Card[TR]]),
    TarjanStack(new arrayindex_t[Net::Card[TR]]),
    mustBeIncluded(new arrayindex_t *[Net::Card[TR]]),
    visited(new uint32_t[Net::Card[TR] ]()),
    onTarjanStack(new uint32_t[Net::Card[TR] ]()),
    stamp(0),unionfind(new int32_t[Net::Card[TR]]),
    status(new uint32_t[Net::Card[TR]+2*Net::Card[PL]]), 
    content(new arrayindex_t[Net::Card[TR]]),
    start(new arrayindex_t[Net::Card[TR]]),
    card(new arrayindex_t[Net::Card[TR]])
{
	// Our And/Or graph contains the following types of nodes:
	// EN: enabled transition    (index = index of t)	
	// DIS: disabled transition  (index = index of t)
	// CON: conflict place   (index = 2* index of p  + card[TR)]
	// SC: scapegoat place   (index = 2* index of p + 1 + card[TR])
	//
	// EN and DIS are disjoint but CON and SC are not necessarily 
	// disjoint: if an enabled tr is in conflict to a disabled transition
	// tr', the conflict place p between tr and tr' can also be scapegoat
	// for tr'!
	// for this reason, status has size card[TR]+2*card[PL]


	// These nodes have the following successors:
	// EN: pre-places (in CON)
	// DIS: pre-places (in SC)
	// CON: post-places (in DIS; only these need to be stored)
	// SC: pre-places (in EN cup DIS)
	// conflicts between enabled transitions are taken care of by
	// clustering them ("all in" or "all out")

	arrayindex_t sss = 0; // next unassigned slot in Post table
	arrayindex_t ssss = 0; // next unassigned slot in Pre table

	cardPost = new arrayindex_t[Net::Card[TR]+2*Net::Card[PL]];
	cardPre = new arrayindex_t[Net::Card[TR]+2*Net::Card[PL]];
	startPre = new arrayindex_t[Net::Card[TR]+2*Net::Card[PL]];
	startPost = new arrayindex_t[Net::Card[TR]+2*Net::Card[PL]];

	// assign segment for post arcs for transitions
	for(arrayindex_t t = 0; t < Net::Card[TR];t++)
	{
		// mustbeincluded(EN) --> CON \subseteq PRE(t)
		// mustbeincluded(DIS) --> SC \subseteq PRE(t)
		// --> at most Pre(t) arcs in Post(t)
		startPost[t] = sss;
		sss += Net::CardArcs[TR][PRE][t];
	
		// EN \subseteq mustbeincluded(SC) (Post) )
		// DIS \subseteq mustbeincluded(SC) (Post\cup mustbeincluded(CON) (Pre)
	 	// at most 
		startPre[t] = ssss;
		ssss += Net::CardArcs[TR][POST][t]+ Net::CardArcs[TR][PRE][t];
	}
	for(arrayindex_t p = 0; p < Net::Card[PL];p++)
	{
		// CON
		
		startPost[Net::Card[TR]+2*p] = sss;
		sss += Net::CardArcs[PL][POST][p];
		
		startPre[Net::Card[TR]+2*p] = ssss;
		ssss += Net::CardArcs[PL][POST][p];
		
		// SC
	
		startPost[Net::Card[TR]+2*p+1] = sss;
		sss += Net::CardArcs[PL][PRE][p];

		startPre[Net::Card[TR]+2*p+1] = ssss;
		ssss += Net::CardArcs[PL][POST][p];
	

	}
	
	Post = new arrayindex_t[sss];
	Pre = new arrayindex_t[ssss];
}

/*!
 * \brief Destructor.
 */
FirelistStubbornCombinedDeadlock::~FirelistStubbornCombinedDeadlock()
{
    delete[] dfsStack;
    delete[] dfs;
    delete[] lowlink;
    delete[] currentIndex;
    delete[] TarjanStack;
    delete[] mustBeIncluded;
    delete[] visited;
    delete[] onTarjanStack;
}

void FirelistStubbornCombinedDeadlock::newStamp()
{
    // 0xFFFFFFFF = max uint32_t
    if (UNLIKELY(++stamp == 0xFFFFFFFF))
    {
        // This happens rarely and only in long runs. Thus it is
        // hard to be tested
        // LCOV_EXCL_START
        for (arrayindex_t i = 0; i < Net::Card[TR]; ++i)
        {
            visited[i] = 0;
            onTarjanStack[i] = 0;
        }
        stamp = 1;
        // LCOV_EXCL_STOP
    }
}


/*!
 * \brief The function to be called when a stubborn set at a given marking
 *        should be constructed
 * \param ns The marking for which the stubborn set should be built.
 * \param result A pointer to NULL. Will be replaced by a pointer to an array
 *               containing the stubborn set.
 * \return The number of element in the stubborn set.
 */
arrayindex_t FirelistStubbornCombinedDeadlock::getFirelist(NetState &ns, arrayindex_t **result)
{
    // Step 1: Take care of case where no transition is enabled
    // This branch is here only for the case that exploration continues
    // after having found a deadlock. In current LoLA, it cannot happen
    // since check property will raise its flag before firelist is
    // requested
    // LCOV_EXCL_START
    if (UNLIKELY(ns.CardEnabled == 0))
    {
        assert(false);
        * result = new arrayindex_t[1];
        return 0;
    }
    // LCOV_EXCL_STOP

    // Step 2: find 1st enabled transition and return immediately 
    // singleton stubborn set if found transition
    // is the only transition in its conflict cluster
    // The list ensures that transitions from small condlict clusters
    // get priority over transitions in large conflict clusters.
    // Relevant preprocessing has taken place in DeadlockTask::buildTask().

    int firstenabled;
    for(firstenabled = 0; firstenabled < Net::Card[TR]; firstenabled++)
    {
        if(ns.Enabled[Transition::StubbornPriority[firstenabled]])
        {
                break;
        }
    }
    // If start transition is alone in its conflict cluster, we
    // can immediately return it as singleton stubborn set.

    if(firstenabled < Transition::SingletonClusters)
    {
        * result = new arrayindex_t[1];
        ** result = Transition::StubbornPriority[firstenabled];
        return 1;
    }

    // STEP 3: Now we execute the actual insertion agorithm in full
    // length. Its result will serve as initial situation for the 
    // subsequent deletion algorithm

    // switch from index in priority list to transition name
    firstenabled = Transition::StubbornPriority[firstenabled];

    // initialize DFS for stubborn closure
    arrayindex_t nextDfs = 1;
    arrayindex_t stackpointer = 0;
    arrayindex_t tarjanstackpointer = 0;
    arrayindex_t dfsLastEnabled;
    newStamp();

    // A deadlock preserving stubborn set is computed by depth first search
    // in a graph. Nodes are transitions.
    // The root of the graph is an arbitrary enabled transition. The edges
    // form a "must be included relation" where, for an enabled transition,
    // its conflicting transitions must be included, and for a disabled i
    // transition, all pre-transitions of an arbitrary insufficiently marked
    // place, called scapegoat, must be included. The resulting stubborn set
    // consists of all enabed transitions in the first encountered SCC of
    // the graph that contains enabled transitions (i.e. a set of transitions
    // that is closed under "must be included" and has at least one enabled
    // tansition).


    scapegoatNewRound();

    // For detecting SCC, we perform depth-first search
    // with the Tarjan extension
    dfs[firstenabled] = lowlink[firstenabled] = 0;
    dfsStack[0] = TarjanStack[0] = firstenabled;

    // a fresh value of stamp signals that a transition has been visited
    // in this issue of dfs search. Using stamps, we do not need to reset
    // "visited" flags
    visited[firstenabled] = onTarjanStack[firstenabled] = stamp;
    mustBeIncluded[0] = Transition::Conflicting[firstenabled];

    // we record the largest dfs of an encountered enabled transition.
    // This way, we can easily check whether an SCC contains enabled
    // transitions.
    dfsLastEnabled = 0;
    // CurrentIndex cotrols the loop over all edges leaving the current
    // transition. By starting from the largest value and counting
    // backwards, we do not need to memorize the overall number of edges.
    currentIndex[0] = Transition::CardConflicting[firstenabled];

    arrayindex_t currenttransition;

    // depth first search
    // quit when scc with enabled transitions is found.
    while (true) // this loop is exited by break statements
    {
        // consider the transition on top of the stack
        currenttransition = dfsStack[stackpointer];
        if ((currentIndex[stackpointer]) > 0)
        {
            // current transition has another successor: newtransition
            arrayindex_t newtransition = mustBeIncluded[stackpointer][--(currentIndex[stackpointer])];
            if (visited[newtransition] == stamp)
            {
                // transition already seen
                // update lowlink of currenttransition: and stay at
                // currenttransition
                if (onTarjanStack[newtransition] == stamp && dfs[newtransition] < lowlink[currenttransition])
                {
                    lowlink[currenttransition] = dfs[newtransition];
                }
            }
            else
            {
                // transition not yet seen: proceed to newtransition
                dfs[newtransition] = lowlink[newtransition] = nextDfs++;
                visited[newtransition] = onTarjanStack[newtransition] = stamp;
                dfsStack[++stackpointer] = newtransition;
                TarjanStack[++tarjanstackpointer] = newtransition;
                if (ns.Enabled[newtransition])
                {
                    // must include conflicting transitions
                    mustBeIncluded[stackpointer] = Transition::Conflicting[newtransition];
                    currentIndex[stackpointer] = Transition::CardConflicting[newtransition];
                    dfsLastEnabled = nextDfs - 1;

                }
                else
                {

                    arrayindex_t scapegoat = Firelist::selectScapegoat(ns,newtransition);
                    // must include pretransitions of scapegoat
                    mustBeIncluded[stackpointer] = Net::Arc[PL][PRE][scapegoat];
                    currentIndex[stackpointer] = Net::CardArcs[PL][PRE][scapegoat];
                }
            }
        }
        else
        {
            // current transition does not have another successor

            // check for closed scc
            if (dfs[currenttransition] == lowlink[currenttransition])
            {
                // scc closed
                // check whether scc contains enabled transitions
                if (dfsLastEnabled >= dfs[currenttransition])
                {
			// proceed with deletion agorithm
			break;
                }
                else
                {
                    // no enabled transitions
                    // pop current scc from tarjanstack and continue
                    while (currenttransition != TarjanStack[tarjanstackpointer--])
                    {
                    }
                    assert(stackpointer > 0);
                    --stackpointer;

                    // In this case: update of lowlink is not necessary:
                    // We have lowlink[currenttransition] == dfs[currenttransition]
                    // dfsStack[stackpointer] is parent of currenttransition
                    // Hence, it has smaller dfs than currenttransition
                    // Hence, it has smaller lowlink anyway.
                    assert(lowlink[currenttransition] >= lowlink[dfsStack[stackpointer]]);
                    //                    if (lowlink[currenttransition] < lowlink[dfsStack[stackpointer]])
                    //                    {
                    //                        lowlink[dfsStack[stackpointer]] = lowlink[currenttransition];
                    //                    }
                }
            }
            else
            {
                // scc not closed
                assert(stackpointer > 0);
                --stackpointer;
                if (lowlink[currenttransition] < lowlink[dfsStack[stackpointer]])
                {
                    lowlink[dfsStack[stackpointer]] = lowlink[currenttransition];
                }
            }
        }
    }
    
    // STEP 4: Init graph structure for deletion algorithm

    // 4.1: read transitions from scc into status

    for(arrayindex_t q = 0; q < Net::Card[TR]+2*Net::Card[PL];q++)
    {
	status[q] = cardPre[q] = cardPost[q] = 0;
    }
    for(arrayindex_t q = 0; q < Net::Card[TR];q++)
    {
	unionfind[q] = -1;
    }

    counter = startPost[Net::Card[TR]]+4;
    for (arrayindex_t i = tarjanstackpointer; ;i--)
    {
	   arrayindex_t t = TarjanStack[i];
	   status[t] = 2; // status IN
	   
           if (ns.Enabled[t])
           {
		// union with all conflicting, which should be somewhere on
		// Tarjan stack
		
		for(arrayindex_t ci = 0; ci < Transition::CardConflicting[t];ci++)
		{
			// transition c is conflicting to transition t
			arrayindex_t c = Transition::Conflicting[t][ci];
			if(!ns.Enabled[c]) continue; // only enabled are clustered
			status[c] = 2; // inevitably so
			// get set of t
			int32_t a;
			for(a = t; unionfind[a] >= 0; a = unionfind[a]);
			// get set of c
			int32_t b;
			for(b = c; unionfind[b] >= 0; b = unionfind[b]);

			// perform union, r is resulting set
			int32_t r;
			if(a == b)
			{
				// t and c already in same set
				r = a;
			}
			else
			{
				if(unionfind[a] < unionfind[b])
				{
					// a is bigger set, as -card is recorded
					// --> link b to a
					unionfind[a] += unionfind[b];
					r = unionfind[b] = a;

				}
				else
				{
					// b is bigger set
					unionfind[b] += unionfind[a];
					r = unionfind[a] = b;
				}
			}
			// path compression
			for(arrayindex_t x = t; unionfind[x] >= 0;)
			{
				int32_t y = unionfind[x];
				unionfind[x] = r;
				x = y;
			} 
			for(arrayindex_t x = c; unionfind[x] >= 0;)
			{
				int32_t y = unionfind[x];
				unionfind[x] = r;
				x = y;
			} 
		}
           }
	   else
	   {
		if(status[t] == 0) status[t] = 2; // 2 = IN (in stubborn set)
           }
	   if(TarjanStack[i] == currenttransition) break;
	   
    }
    
    // turn unionfind structure into a transition based info array

    card_enabled_clusters = 0;
    card_enabled = 0;
    for (arrayindex_t i = tarjanstackpointer; ;i--)
    {
	arrayindex_t t = TarjanStack[i];
        if(!ns.Enabled[t]) 
	{
		if(t == currenttransition) break;
		continue;
	}
        if(unionfind[t] >= 0) 
	{
		if(t == currenttransition) break;
		continue;
	}

        // here, t is the name of a set of activated transitions
        card_enabled_clusters++;
        start[t] = card_enabled; // at thus index, elements of this set
                                 // will be recorded in content array
	card_enabled -= unionfind[t]; // unionfind records -card(set)
	card[t] = 0; // will be set when reading out whole set

	if(t == currenttransition) break;
    }

    if(card_enabled_clusters == 1)
    {
	// nothing to do for deletion algorithm
        * result = new arrayindex_t[card_enabled];
	arrayindex_t rindex = 0;
        for (arrayindex_t i = tarjanstackpointer; TarjanStack[i] != currenttransition;i--)
	{
		arrayindex_t t = TarjanStack[i];
		if(ns.Enabled[t]) 
		{
			(*result)[rindex++] = t;
		}
	}
	if(ns.Enabled[currenttransition]) 
        {
		(*result)[rindex++] = currenttransition;
	}
	return card_enabled;
    }
    for (arrayindex_t i = tarjanstackpointer; ;i--)
    {
	arrayindex_t t = TarjanStack[i];
        if(!ns.Enabled[t]) 
	{
		if(t == currenttransition) break;
		continue;
	}
	// get set of t
	arrayindex_t s;
	for(s = t; unionfind[s] >= 0; s = unionfind[s]);
	content[start[s]+(card[s]++)] = t;
	start[t] = start[s];
	if(t != s) card[t] = -unionfind[s];
 	if(t == currenttransition) break;
    }
    //printClusters(ns);
    

    // second round: scan through disabled transitions and set scapegoats.
    // At this point: all transitions in (insertion) stubborn set have 
    // status != 0.
		
    // insert conflicting places and link to disabled conflicting
    // transitions. Place and arcs are only inserted if necessary,
    // that is, for one of the enabled transitions in the cluster

    for(arrayindex_t jj = 0; jj < card_enabled;jj++)
    {
	arrayindex_t t = content[jj];
	// The following value satisfies these requirements:
	// - it is > 1 (that is, will be interpreted as IN in status)
	// - it is fixed and unique for every cluster of enabled
	//   transitions
	arrayindex_t my_statusvalue = start[t]+3; 
        for(arrayindex_t pi = 0; pi < Net::CardArcs[TR][PRE][t];pi++)
        {
		// p is one of the pre-places of t
		arrayindex_t p = Net::Arc[TR][PRE][t][pi];
		
		// post-transitions of p are necessarily in stubborn set
		// for all post-transitions of p...
		for(arrayindex_t ti = 0; ti < Net::CardArcs[PL][POST][p];ti++)
		{
			arrayindex_t tt = Net::Arc[PL][POST][p][ti];

			if(ns.Enabled[tt]) continue; // looking for disabled tt
			if(status[tt] == my_statusvalue) continue; // already linked from some transition in t's cluster


			arrayindex_t con_idx = 2*p+Net::Card[TR];
			// insert con place if not yet there
			if(status[con_idx] != my_statusvalue)
			{
				status[con_idx] = my_statusvalue;
				Post[startPost[t]+(cardPost[t]++)] = con_idx;
				
			}
			status[tt] = my_statusvalue; // now it is linked
			Post[startPost[con_idx]+(cardPost[con_idx])++] = tt;
		}
	}

    }

    for (arrayindex_t i = tarjanstackpointer; ;i--)
    {
	   arrayindex_t t = TarjanStack[i];
	   if(ns.Enabled[t])
	   {
		   if(t == currenttransition) break;
		   continue; // only interested in disabled trans.
           
	   }
	   for(arrayindex_t pi = 0; pi < Net::CardArcs[TR][PRE][t];pi++)
	   {
		arrayindex_t p = Net::Arc[TR][PRE][t][pi];
		
		// p is scapegoat candidate if not sufficiently marked 
	        // and all its pre-transitions are in insertin stubborn set

		if(ns.Current[p] >= Net::Mult[TR][PRE][t][pi]) continue;
		if(status[2*p+1 + Net::Card[TR]])
		{
			// already inserted as scapegoat, just add arc
			Post[startPost[t]+(cardPost[t]++)] = 2*p+1 + Net::Card[TR];
			continue; 
		} 
		bool candidate = true;
		for(arrayindex_t tti = 0; tti < Net::CardArcs[PL][PRE][p]; tti++)
		{
			arrayindex_t ttt = Net::Arc[PL][PRE][p][tti];
			if(status[ttt] == 0)
			{
				candidate = false;
				break;
			}
			
		}
		if(candidate)
		{
			arrayindex_t idx = 2*p+1 + Net::Card[TR];
			status[idx] = 2;
			Post[startPost[t]+(cardPost[t]++)] = idx;
			for(arrayindex_t tti = 0; tti < Net::CardArcs[PL][PRE][p]; tti++)
			{
				arrayindex_t ttt = Net::Arc[PL][PRE][p][tti];
				Post[startPost[idx]+(cardPost[idx]++)] = ttt;
			}
		}
	   }
	   if(t == currenttransition) break;
    }
	//printAndOr(ns);

    // Reverse arcs (i.e. set Pre)
    for(arrayindex_t node = 0; node < Net::Card[TR]+2*Net::Card[PL]; node++)
    {
	if(status[node] == 0) continue;

	for(arrayindex_t aa = 0; aa < cardPost[node]; aa++)
	{
		arrayindex_t nnode = Post[startPost[node]+aa];
		Pre[startPre[nnode]+(cardPre[nnode]++)] = node;

	}
    }
	//printAndOr(ns);
    // STEP 5: run actual deletion procedure;

    // From this point, the following is invariant for status:
    // status = 0: node permanently removed (or never been in) stubborn set
    // status = 1: node is "nailed", that is, not to be removed any more
    // status = 2..(counter-1): node is in stubborn set but not nailed
    // status = counter: node is temporarily removed from stubborn set

    arrayindex_t candidate_transition = 0;
    while(true)
    {
	// search first enabled transition >= candidate.
	// idea: all enabled transitions left of candidate are either
 	// nailed or permanently removed.
	for( ; candidate_transition < Net::Card[TR]; candidate_transition++)
	{
		// temp-removed transitions of previous rounds are IN again
		counter++; 
	//checkConsistency(ns,"new round deletion");
		// start transition should be IN
		if(status[candidate_transition] <= 1) continue;
		// start speculation only at enabled transition
		if(ns.Enabled[candidate_transition]) break; 
	}
	if(candidate_transition >= Net::Card[TR])
	{
		// no further candidate for removal -->
		// return stubborn set
		* result = new arrayindex_t [card_enabled];
		arrayindex_t f = 0;
		for(arrayindex_t i = 0; i < Net::Card[TR];i++)
		{
			if(status[i] == 0) continue;
			if(!ns.Enabled[i]) continue;
			(*result)[f++] = i;
		}
		assert(f == card_enabled);
		return card_enabled;
	}
	temp_card_enabled = card_enabled;
	temp_card_enabled_clusters = card_enabled_clusters;

	//checkConsistency(ns,"before spec");
	if(speculate(ns,candidate_transition)) 
	{
	//checkConsistency(ns,"after successful spec");
		// candidate_transition + consequences can be safely removed
		card_enabled = temp_card_enabled;
		card_enabled_clusters = temp_card_enabled_clusters;
		if(card_enabled_clusters == 1)
		{
	
			// no further candidate for removal -->
			// return stubborn set
			* result = new arrayindex_t [card_enabled];
			arrayindex_t f = 0;
			for(arrayindex_t i = 0; i < Net::Card[TR];i++)
			{
				if(status[i] == 0) continue;
				if(!ns.Enabled[i]) continue;
				if(status[i] == counter) continue;
				(*result)[f++] = i;
			}
			assert(f == card_enabled);
			return card_enabled;
		}
		for(arrayindex_t k = 0; k < Net::Card[TR] + 2 * Net::Card[PL];k++)
		{
			// mark temp out as perm out
			if(status[k] == counter) status[k] = 0;
		}
	}
	else
	{
		nail(ns,candidate_transition); // let candidate_transition+consequences be NAILED (permanently IN)
	}
    }
}

void andorprintname(arrayindex_t);
bool FirelistStubbornCombinedDeadlock::speculate(NetState & ns,arrayindex_t node)
{
	//RT::rep->status("speculating on node");
	//andorprintname(node);
	//RT::rep->status("=============");
	//printAndOr(ns);
	//checkConsistency(ns,"in spec");
	//RT::rep->status("=============");
	if(status[node] == 1) return false; // trying to remove nailed node
	if(status[node] == 0) return true; // trying to remove absent node
	if(status[node] == counter) return true; // trying to remove removed node
	//RT::rep->status("start to work");
	if(node < Net::Card[TR])
	{
		// node is transition
		if(ns.Enabled[node])
		{
			//RT::rep->status("EN");
			if(temp_card_enabled_clusters-- == 1) 
			{
				// trying to remove the last enabled transtions
				return false;
			}
			// node is enabled transition
			// 1. set whole cluster to tmp_out
			temp_card_enabled -= card[node];
			for(arrayindex_t i = start[node]; i < start[node]+card[node];i++)
			{
				status[content[i]] = counter;
			}
			// 2. propagate via Pre
			// for all transitions t in cluster...
			for(arrayindex_t i = start[node]; i < start[node]+card[node];i++)
			{
				arrayindex_t t = content[i];
				// for all pre-nodes of t...
				for(arrayindex_t j = startPre[t]; j < startPre[t]+cardPre[t]; j++)
				{
					if(!speculate(ns,Pre[j])) return false;
				}
			}
			return true;
		}
		else
		{
		//RT::rep->status("DIS");
			// node is disabled transition
			// 1. set node to tmp_out
			status[node] = counter; 
			// 2. propagate via Pre
			for(arrayindex_t j = startPre[node]; j < startPre[node]+cardPre[node]; j++)
			{
				if(!speculate(ns,Pre[j])) return false;
			}
			return true;
		}
	}
	else	
	{
		// Node is place
		if(!((node - Net::Card[TR])%2))
		{
		//RT::rep->status("CON");
			// node is conflicting place
			// 1. set node to tmp_out
			status[node] = counter; 
			// 2. propagate via Pre
			for(arrayindex_t j = startPre[node]; j < startPre[node]+cardPre[node]; j++)
			{
				if(!speculate(ns,Pre[j])) return false;
			}
			return true;
		}
		else
		{
		//RT::rep->status("SC");
			// node is scapegoat place
			// 1. set node to tmp_out
			status[node] = counter; 
			// 2. foreach t in Pre (disabled transition) ...
			for(arrayindex_t j = startPre[node]; j < startPre[node]+cardPre[node]; j++)
			{
				arrayindex_t t = Pre[j];
				assert(!ns.Enabled[t]);
				// ...check whether node is last scapegoat
				bool hasscapegoat = false;
				for(arrayindex_t jj = startPost[t]; jj< startPost[t]+cardPost[t];jj++)
				{
					arrayindex_t p = Post[jj];
					if((status[p] != 0) && (status[p] != counter))
					{	
						hasscapegoat = true;
						break;
					}
				}
				if(!hasscapegoat) // removing last sc
				{
					if(!speculate(ns,t)) return false;
				}
			}
			return true;
		}
	}
}

void FirelistStubbornCombinedDeadlock::nail(NetState & ns,arrayindex_t node)
{
	// mark nodes a "permanently in" after failed speculation
	// propagate this via Post

	assert(status[node] != 0);

	if(status[node] == 1) return; // already nailed

	if(node < Net::Card[TR])
	{
		// node is transition
		if(ns.Enabled[node])
		{
			// node is enabled transition
			for(arrayindex_t jj = start[node]; jj < start[node]+card[node];jj++)
			{
				status[content[jj]] = 1;
			}
			for(arrayindex_t jj = start[node]; jj < start[node]+card[node];jj++)
			{
				arrayindex_t t = content[jj];
				for(arrayindex_t kk = startPost[t]; kk <startPost[t]+cardPost[t];kk++)
				{
					nail(ns,Post[kk]);
				}
			}
		}
		else
		{
			// node is disabled transition
			
			status[node] = 1;
			// check if node has only one scapegoat
			arrayindex_t cardsc = 0;
			arrayindex_t sc;
			
			for(arrayindex_t ll = startPost[node]; ll < startPost[node]+cardPost[node];ll++)
			{
				if(status[Post[ll]] == 0) continue;
				cardsc++;
				sc = Post[ll];
			}
			assert(cardsc > 0);
			if(cardsc == 1)
			{
				nail(ns,sc);
			}
			
		}
	}
	else
	{
		// node is place
		status[node] = 1;
		for(arrayindex_t mm = startPost[node]; mm < startPost[node]+cardPost[node];mm++)
		{
			nail(ns,Post[mm]);
		}
		
	}
}

Firelist *FirelistStubbornCombinedDeadlock::createNewFireList(SimpleProperty *)
{
    return new FirelistStubbornCombinedDeadlock();
}

// debug function

void andorprintname(arrayindex_t n)
{
	if(n < Net::Card[TR]) 
	{
		RT::rep->status("node %s:",Net::Name[TR][n]);
		return;
	}
	if(n < Net::Card[TR]+2* Net::Card[PL])
	{
		RT::rep->status("node %s:",Net::Name[PL][(n-Net::Card[TR])/2]);
		return;
	}
	RT::rep->status("node: illegal index");
}
void FirelistStubbornCombinedDeadlock::printAndOr(NetState & ns)
{
	RT::rep->status("AND/OR");
	for(arrayindex_t n = 0; n < Net::Card[TR]+2*Net::Card[PL]; n++)
	{
		// print name and status
		
		if(status[n] != 0)
		{
			andorprintname(n);
			const char * statustext;
			if(status[n] == 1)
			{
				statustext = "NAILED";
			}
			else
			{
				if(status[n] == counter)
				{
					statustext = "TEMPOUT";
				}
				else
				{
					if((status[n] > 1) && (status[n] < counter))
					{
						statustext = "IN";
					}
					else
					{
						statustext = "ILLEGAL";
					}
				}
			}
			if(n < Net::Card[TR])
			{
				// node is transition
				if(ns.Enabled[n])
				{
					RT::rep->status("EN %llu %s",status[n],statustext);
				}
				else
				{
					RT::rep->status("DIS %llu %s",status[n],statustext);
				}
			}
			else
			{
				if(!((n - Net::Card[TR])%2))
				{
					RT::rep->status("CON %llu %s",status[n],statustext);
				}
				else
				{
					RT::rep->status("SC %llu %s",status[n], statustext);
				}
			}
			RT::rep->status("post %lu",cardPost[n]);
			for(arrayindex_t i = startPost[n]; i < startPost[n]+cardPost[n];i++)
			{
				andorprintname(Post[i]);
			}
			RT::rep->status("pre %lu",cardPre[n]);
			for(arrayindex_t i = startPre[n]; i < startPre[n]+cardPre[n];i++)
			{
				andorprintname(Pre[i]);
			}
		}
	}
}

void FirelistStubbornCombinedDeadlock::printClusters(NetState & ns)
{
	RT::rep->status("Clusters");
	for(arrayindex_t n = 0; n < Net::Card[TR];n++)
	{
		if(status[n] == 0) continue;
		if(!ns.Enabled[n]) continue;
		for(arrayindex_t i = start[n]; i < start[n]+card[n];i++)
		{
			andorprintname(content[i]);
		}
		RT::rep->status(" ");
	}
}

void FirelistStubbornCombinedDeadlock::checkConsistency(NetState & ns,char * text)
{
	
	RT::rep->status("consistency check %s",text);
	// for all nodes in AND/OR graph
	for(arrayindex_t i = 0; i < Net::Card[TR] + 2 * Net::Card[PL];i++)
	{
		if((status[i] == 0) || (status[i] == counter)) continue;

		if(i < Net::Card[TR])
		{
			// node is transition
			arrayindex_t t = i;
			if(ns.Enabled[t])
			{
				// t enabled: check whether conflicting are 
				// present
				for(arrayindex_t j = 0; j < Transition::CardConflicting[t];j++)
				{
					arrayindex_t tt = Transition::Conflicting[t][j];
					if((status[tt] == 0) || (status[tt] == counter)) RT::rep->status("Enabled transition %s misses conflicting transition %s",Net::Name[TR][t],Net::Name[TR][tt]);
				}
			}
			else
			{
				// t disabled: check for scapegoat
				arrayindex_t j;
				for(j = 0; j < Net::CardArcs[TR][PRE][t];j++)
				{
					arrayindex_t sc = Net::Arc[TR][PRE][t][j];
					if(ns.Current[sc] >= Net::Mult[TR][PRE][t][j]) continue;
					arrayindex_t idx = Net::Card[TR]+2*sc+1;
					if((status[idx] == 0) || status[idx] ==counter) continue;			break;
				}
				if(j >= Net::CardArcs[TR][PRE][t]) RT::rep->status("Disabled transition %s misses scapegoat",Net::Name[TR][t]);
				
			}
		}
		else
		{
			// node is place
			arrayindex_t p = (i - Net::Card[TR]) / 2;
			if((i - Net::Card[TR])%2) 
			{
				// p is scapegoat->check pre-set
				for(arrayindex_t j = 0; j < Net::CardArcs[PL][PRE][p];j++)
				{
					arrayindex_t t = Net::Arc[PL][PRE][p][j];
					if((status[t] == 0) || (status[t] == counter)) RT::rep->status("scapegoat %s misses pre-transition %s",Net::Name[PL][p],Net::Name[TR][t]);
				}
			}
		}
	}
	RT::rep->status("end of consistency check");
}

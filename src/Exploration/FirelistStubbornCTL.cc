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

\brief Class for stubborn CTL firelist generation. We use a quick&dirty approach. We are just looking for a transition in a singleton conflict cluster that is
invisible. If so, it satisfies all requirements for CTL preserving stubborn 
sets (except, of course, ignorance that needs to be handled upon closing scc)
*/

#include <Core/Dimensions.h>
#include <Core/Runtime.h>
#include <Exploration/Firelist.h>
#include <Exploration/FirelistStubbornCTL.h>
#include <Net/Net.h>
#include <Net/NetState.h>
#include <Net/Transition.h>

FirelistStubbornCTL::FirelistStubbornCTL() 
{}

FirelistStubbornCTL::~FirelistStubbornCTL()
{
}

arrayindex_t FirelistStubbornCTL::getFirelist(NetState &ns, arrayindex_t **result)
{
    // STEP 1: take care of no transition enabled
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

    // STEP 2: find 1st enabled transition according to priority list.
    // The list ensures that transitions from small condlict clusters
    // get priority over transitions in large conflict clusters.
    // Relevant preprocessing has taken place in DeadlockTask::buildTask().
   
    arrayindex_t firstenabled;
    arrayindex_t t;
    for(firstenabled = 0; firstenabled < Net::Card[TR]; firstenabled++)
    {
	t = Transition::StubbornPriority[firstenabled];
	if(Transition::Visible[t]) continue;
	if(ns.Enabled[t])
	{
		break;
	}
    }

    // STEP 3: If start transition is alone in its conflict cluster, we
    // can immediately return it as singleton stubborn set.
 
    if(firstenabled < Transition::SingletonClusters)
    {
	* result = new arrayindex_t[1];
	** result = t;
	return 1;
    }

    // STEP 4: Otherwise, return all enabled transitions
    assert(ns.CardEnabled <= Net::Card[TR]);

    *result = new arrayindex_t[ns.CardEnabled];
    arrayindex_t i = 0;
    for (t = 0; t < Net::Card[TR]; ++t)
    {
        if (ns.Enabled[t])
        {
            assert(i < ns.CardEnabled);
            (*result)[i++] = t;
        }
    }
    return ns.CardEnabled;
}

Firelist *FirelistStubbornCTL::createNewFireList(SimpleProperty *)
{
    return new FirelistStubbornCTL();
}

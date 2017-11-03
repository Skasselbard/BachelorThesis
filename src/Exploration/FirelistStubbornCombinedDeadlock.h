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

\brief Class for firelist generation by the combined insertion/deletion algorithm for stubborn
sets for deadlock search.
*/

#pragma once

#include <Core/Dimensions.h>

class Firelist;

/// a stubborn firelist for the search for deadlocks 
class FirelistStubbornCombinedDeadlock : public Firelist
{
public:
    /// constructor for deadlock search
    FirelistStubbornCombinedDeadlock();

    /// destructor
    ~FirelistStubbornCombinedDeadlock();

    /// return value contains number of elements in fire list, argument is reference
    /// parameter for actual list
    virtual arrayindex_t getFirelist(NetState &ns, arrayindex_t **);

virtual Firelist *createNewFireList(SimpleProperty *property);

private:
    // data structure for initial insertion algorithm
    arrayindex_t *dfsStack;
    arrayindex_t *dfs;
    arrayindex_t *lowlink;
    arrayindex_t *currentIndex;
    arrayindex_t *TarjanStack;
    arrayindex_t **mustBeIncluded;
    uint32_t *visited;
    uint32_t *onTarjanStack;
    uint32_t stamp;
    void newStamp();
    // data structure for subsequent deletion algorithm
    int32_t * unionfind; // for aggregating enabled transitions
    uint32_t * status;  // in, out, nailed, or tmp_out in deletion alg.
    arrayindex_t * content; // lists aggregated enabled transition consecutively
    arrayindex_t * start; // first element of my cluster in content
    arrayindex_t * card; // number of elements in my cluster in content
    arrayindex_t * cardPost; // number of outgoing arcs of this node in and/or graph
    arrayindex_t * cardPre; // number of incoming arcs  of this node in and/or graph
    arrayindex_t * startPost; // first outgoing arc in Post arc list
    arrayindex_t * startPre; // first incoming arc in Post arc list
    arrayindex_t * Post; // list of outgoing arcs
    arrayindex_t * Pre; // list of incoming arcs
    bool speculate(NetState &,arrayindex_t); // try to remove node + consequences
    void nail(NetState &,arrayindex_t); // mark t as unremovable + consequences
    arrayindex_t card_enabled;
    arrayindex_t card_enabled_clusters;
    arrayindex_t temp_card_enabled;
    arrayindex_t temp_card_enabled_clusters;
    uint64_t counter;
    void printAndOr(NetState &);
    void printClusters(NetState &);
    void checkConsistency(NetState &,char *);
};
	

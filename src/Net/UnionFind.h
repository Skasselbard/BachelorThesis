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
\author Markus
\status new

\brief Data Structures for Union Find

*/

#pragma once

#include <Core/Dimensions.h>

class UnionFind
{
    public:
    //the smallest number( highest negative) which a root cluster has
    static int rootNodeMax;

    //the number of elements in the Conflictcluster array    
    static arrayindex_t clusterSize;

    //initialise the Conflictcluster in Net::UnionFind
    static void initCluster();

    //calculate the Conflictcluster for the current Net
    static void calculateConflictCluster();
    
    // find the corresponding  root_node eg. the cluster "name"
    static arrayindex_t find(arrayindex_t,node_t);

    // print the Conflictcluster
    static void printCluster();

};

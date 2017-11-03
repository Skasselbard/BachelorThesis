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

\brief class for firelist generation. Implements traditional stubborn sets
for LTL using the idea of strongly connected components.
*/

#pragma once

#include <Core/Dimensions.h>

class Firelist;

/// A stubborn set (firelist) for the check of LTL formulas.
class FirelistStubbornLTLTarjan : public Firelist
{
public:
    explicit FirelistStubbornLTLTarjan();
    ~FirelistStubbornLTLTarjan();
    
    /// Return the number of elements in the stubborn set (firelist). The
    /// parameter is reference for the actual list.
    virtual arrayindex_t getFirelist(NetState &ns, arrayindex_t **);

    /// Create a new stubborn set (firelist) for LTL formulas form the current 
    /// one.
    virtual Firelist *createNewFireList();

private:
    arrayindex_t *dfsStack;
    arrayindex_t *dfs;
    arrayindex_t *lowlink;
    arrayindex_t *currentIndex;
    arrayindex_t *TarjanStack;
    arrayindex_t **mustBeIncluded;
    uint32_t *visited;
    uint32_t *onTarjanStack;
    uint32_t stamp;
    bool * tried;
    void newStamp();
};

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
#pragma once
/*!
\file
\author Karsten
\status new

\brief temporal list based container for terms in atomic propositions

*/

#include<Core/Dimensions.h>

class Term
{
	public:
	Term();
	~Term();
	arrayindex_t place;  // index of place. If set to Net::Card[PL]:
				// numerical constant
	int32_t mult;
	Term * next;
	arrayindex_t card;

	void multiply(int32_t);
	void append(Term *);
	Term * copy();
	void print();
};

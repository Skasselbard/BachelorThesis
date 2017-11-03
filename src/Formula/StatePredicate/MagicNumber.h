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

\brief We assign a magic number to every statepredicate. The number has
the following properties:
- if it is equal for two predicates, the predicates are equivalent
- if they are inverse (k1 = -k2), one is the negation of the other
- otherwise, we do not guarantee anything.

We achieve this property through the following scheme

predicate            magic number
---------------------------------
TRUE			1
FALSE			-1
DEADLOCK		2
NODEADLOCK		-2
FIREABLE(t)		t+3
UNFIREABLE(t)		-(t+3)
p <= 0			|T| + 3 + p
p >= 1  		-(|T| + 3 + p)

Otherwise		new, previously ununsed number.

The magic number is transferred through operations as follows:
- kept in copy()
- inversed in negate()
- kept in special operations, such as transferring a fireable predicate into
  a corresponding conjunction...

*/

#pragma once

#include <Net/Net.h>
struct MagicNumber
{
	static int64_t nextMagicNumber;
	static int64_t assign();
};

#define MAGIC_NUMBER_TRUE ((int64_t)(1))
#define MAGIC_NUMBER_FALSE (-(int64_t)(1))
#define MAGIC_NUMBER_DEADLOCK ((int64_t)(2))
#define MAGIC_NUMBER_NODEADLOCK (-(int64_t)(2))
#define MAGIC_NUMBER_FIREABLE(T) ((T) + 3)
#define MAGIC_NUMBER_UNFIREABLE(T) (-(int64_t)(((T) + 3)))
#define MAGIC_NUMBER_EMPTY(P) (Net::Card[TR] + 3 + (P))
#define MAGIC_NUMBER_MARKED(P) (-(int64_t)((Net::Card[TR] + 3 + (P))))
#define MAGIC_NUMBER_INITIAL (Net::Card[TR] + Net::Card[PL] + 3)

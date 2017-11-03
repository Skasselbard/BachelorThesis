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
\status approved 23.05.2012, changed

\brief Evaluates simple property (only SET of states needs to be computed).
Actual property is a parameter of the constructor
*/

#include <Core/Dimensions.h>
#include <CoverGraph/CoverGraph.h>
#include <Exploration/ComputeBoundExploration.h>
#include <Exploration/StatePredicateProperty.h>
#include <Exploration/ChooseTransition.h>
#include <Net/LinearAlgebra.h>
#include <Net/Marking.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Transition.h>
#include <SweepLine/Sweep.h>
#include <Formula/StatePredicate/AtomicStatePredicate.h>

#include "../../libs/lp_solve_5.5/lp_lib.h"

/*!
The result will be the maximum (over all reachable markings) of the given formal sum of places.

\param property  contains the expression to be checked as atomic proposition
\param ns  The initial state of the net has to be given as a net state object.
\param myStore  the store to be used. The selection of the store may greatly influence the
performance of the program
\param myFirelist  the firelists to use in this search. The firelist _must_ be
applicable to the given property, else the result of this function may be
wrong. It is not guaranteed that the given firelist will actually be used. In
the parallel work-mode the given list will just be used as a base list form
which all other lists will be generated
\param threadNumber  will be ignored by the standard seach. In the parallel
execution mode this number indicates the number of threads to be used for the
search
*/
int64_t ComputeBoundExploration::depth_first_num(SimpleProperty &property, NetState &ns, Store<void> &myStore,
        Firelist &myFirelist, int)
{
	int64_t StructuralBound = lp(property,ns,myStore,myFirelist,1);
	RT::rep->status("Structural Bound: %ld",StructuralBound);
	StatePredicateProperty * sp = reinterpret_cast<StatePredicateProperty*>(&property);
	sp -> createDownSets(sp -> predicate);
	AtomicStatePredicate * a = reinterpret_cast<AtomicStatePredicate *>(sp -> predicate);
	//// copy initial marking into current marking
	//Marking::init();

	// prepare property
	property.value = property.initProperty(ns);
	result = a -> sum;
	if(result -a -> threshold== StructuralBound) return result - a -> threshold;

	// add initial marking to store
	// we do not care about return value since we know that store is empty

	myStore.searchAndInsert(ns, 0, 0);

	// get first firelist
	arrayindex_t *currentFirelist;
	arrayindex_t currentEntry = myFirelist.getFirelist(ns, &currentFirelist);
	while (true) { // exit when trying to pop from empty stack
		if (currentEntry-- > 0) {
			// there is a next transition that needs to be explored in current marking

			// fire this transition to produce new Marking::Current
			Transition::fire(ns, currentFirelist[currentEntry]);

			if (myStore.searchAndInsert(ns, 0, 0)) {
				// State exists! -->backtracking to previous state
				Transition::backfire(ns, currentFirelist[currentEntry]);
			} else {
				// State does not exist!
				Transition::updateEnabled(ns, currentFirelist[currentEntry]);
				// check current marking for property
				property.value = property.checkProperty(ns, currentFirelist[currentEntry]);
				if (a->sum > result) {
					result = a -> sum;
					if(result - a->threshold == StructuralBound) return result - a->threshold;
				}

				SimpleStackEntry *stack = property.stack.push();
				stack = new(stack) SimpleStackEntry(currentFirelist, currentEntry);
				currentEntry = myFirelist.getFirelist(ns, &currentFirelist);
			} // end else branch for "if state exists"
		} else {
			// firing list completed -->close state and return to previous state
			delete[] currentFirelist;
			if (property.stack.StackPointer == 0) {
				// have completely processed initial marking --> state not found
				return result - a -> threshold;
			}
			SimpleStackEntry &stack = property.stack.top();
			currentEntry = stack.current;
			currentFirelist = stack.fl;
			stack.fl = NULL;
			property.stack.pop();
			assert(currentEntry < Net::Card[TR]);
			Transition::backfire(ns, currentFirelist[currentEntry]);
			Transition::revertEnabled(ns, currentFirelist[currentEntry]);
			property.value = property.updateProperty(ns, currentFirelist[currentEntry]);
		}
	}
}

/* compute the upper bound using linear programming, lp_solve */
int64_t ComputeBoundExploration::lp(SimpleProperty &property, NetState &ns, Store<void> &myStore,
                                    Firelist &myFirelist, int)
{
	StatePredicateProperty * sp = reinterpret_cast<StatePredicateProperty*>(&property);
	sp -> createDownSets(sp -> predicate);
	AtomicStatePredicate * a = reinterpret_cast<AtomicStatePredicate *>(sp -> predicate);

	arrayindex_t *posPlaces = a -> posPlaces;
	arrayindex_t *negPlaces = a -> negPlaces;
	arrayindex_t cardPos = a -> cardPos;
	arrayindex_t cardNeg = a -> cardNeg;
	capacity_t *posMult = a -> posMult;
	capacity_t *negMult = a -> negMult;

	//create the lp and solve it
	lprec *lp;
	int Ncol, *colno = NULL, j, ret = 0;
	REAL *row = NULL;
	Ncol = Net::Card[PL] + Net::Card[TR];
	lp = make_lp(0, Ncol);
	if(lp == NULL)
		ret = 1; /* couldn't construct a new model... */

	if(ret == 0) {

		/* create space large enough for one row */
		colno = (int *) malloc(Ncol * sizeof(*colno));
		row = (REAL *) malloc(Ncol * sizeof(*row));
		if((colno == NULL) || (row == NULL))
			ret = 2;
	}
	/* The enumeration of variables is as follows: p0~1, p1~2,...,pMAX_PLACES-1~MAX_PLACES,t0~MAX_PLACES+1, t1~MAX_PLACES+2,... */

	/* set all variables to integer */
	for (int i=1; i<=Net::Card[PL]; i++) {
		set_int(lp, i, TRUE);
	}
	for (int i=Net::Card[PL] + 1; i<=Net::Card[TR]; i++) {
		set_int(lp, i, TRUE);
	}

	if(ret == 0) {
		set_add_rowmode(lp, TRUE);  /* makes building the model faster if it is done rows by row */
	}

	/* add the constraits row by row. These are the rows of the state equation*/
	for (int i=0; i<Net::Card[PL]; i++) {
		j = 0;
		int num_tin = Net::CardArcs[PL][PRE][i];
		for (int k=0; k<num_tin; k++) {
			int tin = Net::Arc[PL][PRE][i][k];
			int mult = Net::Mult[PL][PRE][i][k];
			colno[j] = Net::Card[PL] + tin + 1;
			row[j++] = mult;
		}

		int num_tout = Net::CardArcs[PL][POST][i];
		for (int k=0; k<num_tout; k++) {
			int tout = Net::Arc[PL][POST][i][k];
			int mult = Net::Mult[PL][POST][i][k];
			colno[j] = Net::Card[PL] + tout + 1;
			row[j++] = -1 * mult;
		}

		colno[j] = i+1;
		row[j++] = -1;
		int m0 = Marking::Initial[i];
		if(!add_constraintex(lp, j, row, colno, EQ, -m0))
			ret = 3;
	}

	if(ret == 0) {
		set_add_rowmode(lp, FALSE); /* rowmode should be turned off again when done building the model */

		/* set the objective function */
		j = 0;

		for (int i = 0; i < cardPos; i++) {
			colno[j] = posPlaces[i] +1;
			row[j++] = static_cast<double> (posMult[i]);
		}

		for (int i = 0; i < cardNeg; i++) {
			colno[j] = negPlaces[i] +1;
			row[j++] = -1 * static_cast<double> (negMult[i]);
		}

		/* set the objective in lpsolve */
		if(!set_obj_fnex(lp, j, row, colno))
			ret = 4;
	}

	if(ret == 0) {
		/* set the object direction to maximize */
		set_maxim(lp);

		/*to show the model in lp format on screen */
		//write_LP(lp, stdout);

		/* I only want to see important messages on screen while solving */
		//set_verbose(lp, IMPORTANT);

		/* Now let lpsolve calculate a solution */
		ret = solve(lp);
		if(ret == OPTIMAL)
			ret = 0;
		else
			ret = 5;
	}

	if(ret == 0) {
		/* objective value */
		result = static_cast<int> (get_objective(lp) + 0.5);

		RT::rep->status("Objective value: %d\n", result);

		/* variable values */
		/*get_variables(lp, row);
		for(j = 0; j < Ncol; j++)
			RT::rep->status("%s: %f\n", get_col_name(lp, j + 1), row[j]);*/
	}

	/* free allocated memory */
	if(row != NULL)
		free(row);
	if(colno != NULL)
		free(colno);

	if(lp != NULL) {
		/* clean up such that all used memory by lpsolve is freed */
		delete_lp(lp);
	}

	return result;

}

// LCOV_EXCL_START
Path ComputeBoundExploration::path() const
{
	return * (new Path());
}
// LCOV_EXCL_STOP

#include <Core/Dimensions.h>
#include <Core/Runtime.h>
#include <Exploration/ComputePlacesBounds.h>
#include <Net/Marking.h>
#include <Net/Net.h>

#include "../../libs/lp_solve_5.5/lp_lib.h"

ComputePlacesBounds::ComputePlacesBounds()
{
	placesBounds = new int64_t[Net::Card[PL]]; //todo: undo public?
	placesBounds = ComputePlacesBounds::computePlacesBounds(); //todo: remove and call it if necessary
}

int64_t* ComputePlacesBounds::computePlacesBounds()
{
	int64_t *bounds = new int64_t[Net::Card[PL]];
	std::list<int> places_to_cover; //a list with the places for which we want to find a positive invariant with i(place)>0
	for (int i = 0; i<Net::Card[PL]; i++)
		places_to_cover.push_back(i);
	int count_inv = 0;
	while(!places_to_cover.empty()) {
		int place_to_cover = places_to_cover.front();

		//solve invariants lp
		int *invariant = new int[Net::Card[PL]];
		ComputePlacesBounds::solve_invariant_lp(invariant, place_to_cover);
		count_inv++;

		//ẃhich other places does this invariant cover? store them in places_covered
		std::list<int> places_covered;
		std::list<int>::iterator it = places_to_cover.begin();
		while (it != places_to_cover.end()) {
			int place = *it;

			if (invariant[place] > 0) {
				places_covered.push_back(place);
				it = places_to_cover.erase(it);
			} else
				it++;
		}
	
		int prod_inv_m0 = 0;
		//compute invariant * init_marking
		for(int i = 0; i<Net::Card[PL]; i++)
		prod_inv_m0 += Marking::Initial[i] * invariant[i];
		
		for (std::list<int>::iterator it=places_covered.begin(); it != places_covered.end(); ++it) {
			int place = *it;
			bounds[place] = prod_inv_m0/invariant[place];
		}
	}
	RT::rep->status("Num invariants: %i", count_inv);
	return bounds;
}

void ComputePlacesBounds::solve_invariant_lp(int *invariant, int place_to_cover)
{
	//create the lp and solve it
	lprec *lp;
	int Ncol, *colno = NULL, j, ret = 0;
	REAL *row = NULL;
	Ncol = Net::Card[PL];
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

	/* set all variables to integer */
	for (int p=1; p<=Net::Card[PL]; p++) {
		set_int(lp, p, TRUE);
	}

	if(ret == 0) {
		set_add_rowmode(lp, TRUE);  /* makes building the model faster if it is done rows by row */
	}

	/* add the constraits row by row. These are the rows of the state equation*/
	for (int t=0; t<Net::Card[TR]; t++) {
		j = 0;
		int num_pre_p = Net::CardArcs[TR][PRE][t];
		for (int k=0; k<num_pre_p; k++) {
			int pre_p = Net::Arc[TR][PRE][t][k];
			int mult = Net::Mult[TR][PRE][t][k];
			colno[j] = pre_p + 1;
			row[j++] = -1 * mult;
		}

		int num_post_p = Net::CardArcs[TR][POST][t];
		for (int k=0; k<num_post_p; k++) {
			int post_p = Net::Arc[TR][POST][t][k];
			int mult = Net::Mult[TR][POST][t][k];
			colno[j] = post_p + 1;
			row[j++] = mult;
		}

		if(!add_constraintex(lp, j, row, colno, EQ, 0))
			ret = 3;
	}

	//set, the invariant(place_to_find)>0, since we want to find a positive invariant for place_to_find
	j = 0;
	colno[j] = place_to_cover + 1;
	row[j++] = 1;
	if(!add_constraintex(lp, j, row, colno, GE, 1)) //there is no > possibility, only >= available. So use >= 1 instead of >0
		ret = 3;

	/*for(int p=1; p<=Net::Card[PL]; p++)
		if (p != place_to_find){
			colno
		}*/

	if(ret == 0) {
		set_add_rowmode(lp, FALSE); /* rowmode should be turned off again when done building the model */

		/* set the objective function */
		j = 0;

		for (int p = 1; p <= Net::Card[PL]; p++) {
			colno[j] = p;
			row[j++] = 1;
		}

		/* set the objective in lpsolve */
		if(!set_obj_fnex(lp, j, row, colno))
			ret = 4;
	}

	if(ret == 0) {
		/* set the object direction to maximize */
		set_minim(lp);

		/*to show the model in lp format on screen */
		//write_LP(lp, stdout);

		/* I only want to see important messages on screen while solving */
		set_verbose(lp, IMPORTANT);

		/* Now let lpsolve calculate a solution */
		ret = solve(lp);
		if(ret == OPTIMAL)
			ret = 0;
		else
			ret = 5;
	}

	if(ret == 0) {
		/* objective value */
		//int result = static_cast<int>(get_objective(lp) + 0.5);

		//RT::rep->status("Objective value: %d\n", result);

		double *invariant_double = new double[Net::Card[PL]];
		get_variables(lp, invariant_double);
		for(j = 0; j < Ncol; j++) {
			invariant[j] =  static_cast<int>(invariant_double[j] + 0.5);
			//RT::rep->status("%s: %i\n", get_col_name(lp, j + 1), invariant[j]);
		}
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
}

//returns an upper bound of place
int ComputePlacesBounds::solve_place_bound_lp(int *invariant, int place)
{
	//RT::rep->status("Computing upper bound of place %s", Net::Name[PL][place]);
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

	int prod_inv_m0 = 0;
	//compute invariant * init_marking
	for(int i = 0; i<Net::Card[PL]; i++)
		prod_inv_m0 += Marking::Initial[i] * invariant[i];

	j=0;
	colno[j] = place+1;
	row[j++] = 1;
	if(!add_constraintex(lp, j, row, colno, LE, (double) prod_inv_m0/invariant[place]))
		ret = 3;

	if(ret == 0) {
		set_add_rowmode(lp, FALSE); /* rowmode should be turned off again when done building the model */

		/* set the objective function */
		j = 0;

		colno[j] = place +1;
		row[j++] = 1.0;

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
		set_verbose(lp, IMPORTANT);

		/* Now let lpsolve calculate a solution */
		ret = solve(lp);
		if(ret == OPTIMAL)
			ret = 0;
		else
			ret = 5;
	}

	int bound;

	if(ret == 0) {
		/* objective value */
		bound = static_cast<int> (get_objective(lp) + 0.5);

		//RT::rep->status("MAX(%s): %d\n", Net::Name[PL][place], bound);

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

	return bound;
}

#include "Net/Net.h"

#pragma once

class FormulaStatistics
{
public:
	// path quantifiers
	int A;
	int E;

	// temporal operators
	int X;
	int F;
	int G;
	int U;
	int R;

	// temp boolean
	int tconj;
	int tdisj;
	int tneg;

	// Atomic
	int taut;
	int cont;
	int dl;
	int nodl;
	int fir;
	int unfir;
	int comp;
	int aconj;
	int adisj;
	int aneg;

	// Nodes

	int visible_places;
	int visible_transitions;
	int place_references;
	int transition_references;
	bool * mentioned_place;
	bool * mentioned_transition;
	FormulaStatistics()
	{
		A=E=X=F= G = U = R = 
		tconj=
		tdisj=
		tneg=
		taut=
		cont=
		dl=
		nodl=
		fir=
		unfir=
		comp=
		aconj=
		adisj=
		aneg=
		visible_places=
		visible_transitions=
		place_references=
		transition_references=0;
		mentioned_place = new bool [Net::Card[PL]];
		mentioned_transition = new bool [Net::Card[TR]];
		for(arrayindex_t i = 0; i < Net::Card[PL];i++)
		{
			mentioned_place[i] = false;
		}
		for(arrayindex_t i = 0; i < Net::Card[TR];i++)
		{
			mentioned_transition[i] = false;
		}
	}
	~FormulaStatistics()
	{
		delete [] mentioned_place;
		delete [] mentioned_transition;
	}
};

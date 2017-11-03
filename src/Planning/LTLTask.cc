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
#include <Frontend/Parser/ParserPTNet.h>
#include <Frontend/SymbolTable/Symbol.h>
#include <Frontend/SymbolTable/SymbolTable.h>
#include <config.h>
#include <Core/Dimensions.h>
#include <Planning/StoreCreator.h>
#include <Exploration/StatePredicateProperty.h>
#include <Exploration/Firelist.h>
#include <Exploration/LTLExploration.h>
#include <Exploration/SimpleProperty.h>
#include <Formula/LTL/BuechiFromLTL.h>
#include <Formula/StatePredicate/TruePredicate.h>
#include <Formula/StatePredicate/StatePredicate.h>
#include <Formula/StatePredicate/AtomicStatePredicate.h>
#include <Formula/StatePredicate/ConjunctionStatePredicate.h>
#include <Formula/StatePredicate/DisjunctionStatePredicate.h>
#include <Frontend/Parser/ast-system-k.h>
#include <Frontend/Parser/ast-system-rk.h>
#include <Frontend/Parser/ast-system-unpk.h>
#include <Planning/LTLTask.h>
#include <Stores/Store.h>
#include <Witness/Path.h>
#include <Exploration/FirelistStubbornLTLTarjan.h>
// #include <Exploration/FirelistStubbornLTL.h>

extern ParserPTNet* symbolTables;
extern int ptbuechi_parse();

extern int ptbuechi_lex_destroy();

// input files
extern FILE *ptformula_in;
extern FILE *ptbuechi_in;

// code to parse from a string
struct yy_buffer_state;
typedef yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE ptbuechi__scan_string(const char *yy_str);

extern SymbolTable *buechiStateTable;
extern FILE *tl_out;

// Kimwitu++ objects
extern kc::tFormula TheFormula;
extern kc::tBuechiAutomata TheBuechi;


/*!
\ingroup g_globals
\todo Is this mapping actually needed or was it this just added for debugging
purposes.
 */
std::map<int, StatePredicate *> predicateMap;

void insertBuchiState(int number_of_states)
{
	// the swap only needs to be executed if there are insignificant places
	if(Place::CardSignificant < Net::Card[PL])
	{
		// the trick is not to forget any data structure already
 		// initialized, and used in LTL model checking

		// Chapter 1: The net data structures

		// 1.1: arrays ranging on all places. They need to be
		// enlarged before shifting the entry

		Net::Name[PL] = reinterpret_cast<const char **>(realloc(Net::Name[PL],(Net::Card[PL]+1)*SIZEOF_VOIDP)); 
		Net::Name[PL][Net::Card[PL]] = Net::Name[PL][Place::CardSignificant];
		char * buchitext = reinterpret_cast<char *>(calloc(sizeof(char) , (strlen("state of buchi automaton") + 1)));
		strcpy(buchitext,"state of buchi automaton");
		Net::Name[PL][Place::CardSignificant] = buchitext;
		for (int direction = PRE; direction <= POST; ++direction)
    		{
			Net::CardArcs[PL][direction] = reinterpret_cast<arrayindex_t *>(realloc(Net::CardArcs[PL][direction], (Net::Card[PL]+1) * SIZEOF_ARRAYINDEX_T));
        		Net::CardArcs[PL][direction][Net::Card[PL]] = Net::CardArcs[PL][direction][Place::CardSignificant];
			Net::CardArcs[PL][direction][Place::CardSignificant] = 0;
			Net::Arc[PL][direction] = reinterpret_cast<arrayindex_t **>(realloc(Net::Arc[PL][direction],(Net::Card[PL]+1) * SIZEOF_VOIDP));
        		Net::Arc[PL][direction][Net::Card[PL]] = Net::Arc[PL][direction][Place::CardSignificant];
			Net::Arc[PL][direction][Place::CardSignificant] = NULL;
			Net::Mult[PL][direction] = reinterpret_cast<mult_t **>(realloc(Net::Mult[PL][direction],(Net::Card[PL]+1)*SIZEOF_VOIDP));
        		Net::Mult[PL][direction][Net::Card[PL]] = Net::Mult[PL][direction][Place::CardSignificant];
			Net::Mult[PL][direction][Place::CardSignificant] = NULL;
    		}

		// arrays ranging on transitions
		for (int direction = PRE; direction <= POST; ++direction)
    		{
        		for (arrayindex_t a = 0; a < Net::CardArcs[PL][direction][Net::Card[PL]]; ++a)
        {
            direction_t otherdirection = (direction == PRE) ? POST : PRE;
            const arrayindex_t t = Net::Arc[PL][direction][Net::Card[PL]][a];
            for (arrayindex_t b = 0; b < Net::CardArcs[TR][otherdirection][t]; b++)
            {
                if (Net::Arc[TR][otherdirection][t][b] == Place::CardSignificant)
                {
                    Net::Arc[TR][otherdirection][t][b] = Net::Card[PL];
                }
            }
        }
    }
		// Chapter 2: Place data structures

		// 2.1 hash function for markings

		Place::Hash = reinterpret_cast<hash_t *>(realloc(Place::Hash,(Net::Card[PL]+1)*SIZEOF_HASH_T));
		Place::Hash[Net::Card[PL]] = Place::Hash[Place::CardSignificant];
		// setting the hash factor for the buchi state to 0 means
		// that we can set the state without need to update the
		// hash value
		Place::Hash[Place::CardSignificant] = 0;

		// 2.2 capacity based arrays

		Place::Capacity = reinterpret_cast<capacity_t *>(realloc(Place::Capacity,(Net::Card[PL]+1)*SIZEOF_CAPACITY_T));
		Place::Capacity[Net::Card[PL]] = Place::Capacity[Place::CardSignificant];
		Place::Capacity[Place::CardSignificant] = number_of_states;
		Place::CardBits = reinterpret_cast<cardbit_t *>(realloc(Place::CardBits,(Net::Card[PL]+1)*SIZEOF_CARDBIT_T));
		Place::CardBits[Net::Card[PL]] = Place::CardBits[Place::CardSignificant];
		Place::CardBits[Place::CardSignificant] = Place::Capacity2Bits(number_of_states);
		Place::SizeOfBitVector += Place::CardBits[Place::CardSignificant];

		// Chapter 3: marking based arrays
		// 3.1. current marking

		Marking::Initial = reinterpret_cast<capacity_t *>(realloc(Marking::Initial,(Net::Card[PL]+1)*SIZEOF_CAPACITY_T));
		Marking::Initial[Net::Card[PL]] = Marking::Initial[Place::CardSignificant];
		Marking::Initial[Place::CardSignificant] = 0;
		if(Marking::Current)
		{
			Marking::Current = reinterpret_cast<capacity_t *>(realloc(Marking::Current,(Net::Card[PL]+1)*SIZEOF_CAPACITY_T));
			Marking::Current[Net::Card[PL]] = Marking::Current[Place::CardSignificant];
			Marking::Current[Place::CardSignificant] = 0;
		}
		
		// Chapter 4: transition based arrays

		for (int direction = PRE; direction <= POST; ++direction)
    {
        for (arrayindex_t a = 0; a < Net::CardArcs[PL][direction][Net::Card[PL]]; ++a)
        {
            direction_t otherdirection = (direction == PRE) ? POST : PRE;
            const arrayindex_t t = Net::Arc[PL][direction][Net::Card[PL]][a];
            for (arrayindex_t b = 0; b < Transition::CardDeltaT[otherdirection][t]; b++)
            {
                if (Transition::DeltaT[otherdirection][t][b] == Place::CardSignificant)
                {
                    Transition::DeltaT[otherdirection][t][b] = Net::Card[PL];
                }
            }
        }
    }

	// Chapter 5: data structures initialised in PreProcessing

	// 5.1. UsedAsScapegoat

	Firelist::usedAsScapegoat = reinterpret_cast<arrayindex_t*>(realloc(Firelist::usedAsScapegoat,(1+Net::Card[PL])*SIZEOF_ARRAYINDEX_T));
	Firelist::usedAsScapegoat[Net::Card[PL]] = 0;

	

		// Last chapter: update Card[PL] and CardSignificant
		Net::Card[PL]++;
		Place::CardSignificant++;
	}
	else
	{
		// the trick is not to forget any data structure already
 		// initialized, and used in LTL model checking

		// Chapter 1: The net data structures

		// 1.1: arrays ranging on all places. They need to be
		// enlarged before shifting the entry

		Net::Name[PL] = reinterpret_cast<const char **>(realloc(Net::Name[PL],(Net::Card[PL]+1)*SIZEOF_VOIDP)); 
		char * buchitext = reinterpret_cast<char *>(calloc(sizeof(char) , (strlen("state of buchi automaton") + 1)));
		strcpy(buchitext,"state of buchi automaton");
		Net::Name[PL][Place::CardSignificant] = buchitext;
		for (int direction = PRE; direction <= POST; ++direction)
    		{
			Net::CardArcs[PL][direction] = reinterpret_cast<arrayindex_t *>(realloc(Net::CardArcs[PL][direction], (Net::Card[PL]+1) * SIZEOF_ARRAYINDEX_T));
			Net::CardArcs[PL][direction][Place::CardSignificant] = 0;
			Net::Arc[PL][direction] = reinterpret_cast<arrayindex_t **>(realloc(Net::Arc[PL][direction],(Net::Card[PL]+1) * SIZEOF_VOIDP));
			Net::Arc[PL][direction][Place::CardSignificant] = NULL;
			Net::Mult[PL][direction] = reinterpret_cast<mult_t **>(realloc(Net::Mult[PL][direction],(Net::Card[PL]+1)*SIZEOF_VOIDP));
			Net::Mult[PL][direction][Place::CardSignificant] = NULL;
    		}

		// Chapter 2: Place data structures

		// 2.1 hash function for markings

		Place::Hash = reinterpret_cast<hash_t *>(realloc(Place::Hash,(Net::Card[PL]+1)*SIZEOF_HASH_T));
		// setting the hash factor for the buchi state to 0 means
		// that we can set the state without need to update the
		// hash value
		Place::Hash[Place::CardSignificant] = 0;

		// 2.2 capacity based arrays

		Place::Capacity = reinterpret_cast<capacity_t *>(realloc(Place::Capacity,(Net::Card[PL]+1)*SIZEOF_CAPACITY_T));
		Place::Capacity[Place::CardSignificant] = number_of_states;
		Place::CardBits = reinterpret_cast<cardbit_t *>(realloc(Place::CardBits,(Net::Card[PL]+1)*SIZEOF_CARDBIT_T));
		Place::CardBits[Place::CardSignificant] = Place::Capacity2Bits(number_of_states);
		Place::SizeOfBitVector += Place::CardBits[Place::CardSignificant];

		// Chapter 3: marking based arrays
		// 3.1. current marking

		Marking::Initial = reinterpret_cast<capacity_t *>(realloc(Marking::Initial,(Net::Card[PL]+1)*SIZEOF_CAPACITY_T));
		Marking::Initial[Place::CardSignificant] = 0;
		if(Marking::Current)
		{
			Marking::Current = reinterpret_cast<capacity_t *>(realloc(Marking::Current,(Net::Card[PL]+1)*SIZEOF_CAPACITY_T));
			Marking::Current[Place::CardSignificant] = 0;
		}
		
		// Chapter 4: transition based arrays

	// Chapter 5: data structures initialised in PreProcessing

	// 5.1. UsedAsScapegoat

	Firelist::usedAsScapegoat = reinterpret_cast<arrayindex_t*>(realloc(Firelist::usedAsScapegoat,(1+Net::Card[PL])*SIZEOF_ARRAYINDEX_T));
	Firelist::usedAsScapegoat[Net::Card[PL]] = 0;

		// Last chapter: update Card[PL] and CardSignificant
		Net::Card[PL]++;
		Place::CardSignificant++;
	}
}


/* prints the content of a set for spin */
StatePredicate *buildPropertyFromList(int *pos, int *neg)
{
    std::vector<StatePredicate *> subForms;

    // bad hack from library
    int mod = 8 * SIZEOF_INT;

    for (int i = 0; i < sym_size; i++)
    {
        for (int j = 0; j < mod; j++)
        {
            if (pos[i] & (1 << j))
            {
                // the compiler doesn't have a clue which function i mean, so tell him
                if (atoi(sym_table[mod * i + j]) > 1)
                {
                    subForms.push_back(
                            predicateMap[atoi(sym_table[mod * i + j])]->copy(NULL));
                }
            }
            if (neg[i] & (1 << j))
            {
                if (atoi(sym_table[mod * i + j]) > 1)
                {
		     StatePredicate * x = predicateMap[atoi(sym_table[mod * i + j])]->copy(NULL);
		x = x -> negate();
		    
                    subForms.push_back(x);
                }
            }
        }
    }

    if (subForms.empty())
    {
        return new TruePredicate();
    }

    ConjunctionStatePredicate *result = new ConjunctionStatePredicate(subForms.size());
    for (arrayindex_t i = 0; i < subForms.size(); i++)
    {
        result->addSub(i, subForms[i]);
    }
    return result;
}

LTLTask::LTLTask()
{
	RT::data["task"]["workflow"] = "product automaton";
	taskname = "LTL model checker";
    // process formula
    previousNrOfMarkings = 0;

    if (RT::args.formula_given)
    {
        RT::rep->status("transforming LTL-Formula into a Büchi-Automaton");
	// subsequent rewriting
	//TheFormula = TheFormula -> rewrite(kc::sides);
	//TheFormula = TheFormula -> rewrite(kc::productlists);
	//TheFormula = TheFormula -> rewrite(kc::leq);
	TheFormula = TheFormula -> rewrite(kc::tautology);

        // extract the Node*
        Task::outputFormulaAsProcessed();
	unparsed.clear();
        TheFormula->unparse(myprinter, kc::ltl);

        tl_Node *n = TheFormula->ltl_tree;
        //n = bin_simpler(n);
        assert(n);
        tl_out = stdout;
        trans(n);
        // build the buechi-automation structure needed for LTL model checking
        // put the state predicates
        bauto = new BuechiAutomata();

        // extract the states from the ltl2ba data structures
        if (bstates->nxt == bstates)
        {
            // TODO the search result is FALSE!
            RT::rep->message("Not yet implemented, result FALSE");
            RT::rep->abort(ERROR_COMMANDLINE);
        }

        if (bstates->nxt->nxt == bstates && bstates->nxt->id == 0)
        {
            // TODO the search result is TRUE!
            RT::rep->message("Not yet implemented, result TRUE");
            RT::rep->abort(ERROR_COMMANDLINE);
        }

        bauto->cardStates = 0;
        // map-> final,id
        std::map<int, std::map<int, int > > state_id;
        BState *s;
        BTrans *t;
        for (s = bstates->prv; s != bstates; s = s->prv)
        {
            state_id[s->final][s->id] = bauto->cardStates;
            bauto->cardStates++;
        }

        //RT::rep->message("Buechi-automaton has %d states", bauto->cardStates);
        // now i do know the number of states
        bauto->cardTransitions = new uint32_t[bauto->cardStates]();
        bauto->nextstate = new uint32_t *[bauto->cardStates]();
        bauto->guard = new StatePredicate **[bauto->cardStates]();
        bauto->isStateAccepting = new bool[bauto->cardStates]();

        std::vector<StatePredicate *> neededProperties;

        // read out the datastructure
        int curState = -1;
        int curProperty = 0;
        for (s = bstates->prv; s != bstates; s = s->prv)
        {
            curState++;
            if (s->id == 0)
            {
                // build a TRUE-loop
                bauto->isStateAccepting[curState] = true;
                bauto->cardTransitions[curState] = 1;
                bauto->nextstate[curState] = new uint32_t [1]();
                bauto->guard[curState] = new StatePredicate *[1]();
                bauto->nextstate[curState][0] = curState;
                neededProperties.push_back(new TruePredicate());
                bauto->guard[curState][0] = neededProperties[curProperty];
                curProperty++;
                continue;
            }
            if (s->final == accepting_state)
            {
                bauto->isStateAccepting[curState] = true;
            }

            // build the successor list
            bauto->cardTransitions[curState] = 0;
            for (t = s->trans->nxt; t != s->trans; t = t->nxt)
            {
                // now build the property
                std::vector<StatePredicate *> disjunctionproperty;
                disjunctionproperty.push_back(buildPropertyFromList(t->pos, t->neg));
                BTrans *t1;
                for (t1 = t; t1->nxt != s->trans;)
                {
                    if (t1->nxt->to->id == t->to->id && t1->nxt->to->final == t->to->final)
                    {
                        disjunctionproperty.push_back(buildPropertyFromList(t1->nxt->pos,
                                t1->nxt->neg));
                        t1->nxt = t1->nxt->nxt;
                    } else
                    {
                        t1 = t1->nxt;
                    }
                }

                if (disjunctionproperty.size() == 1)
                {
                    neededProperties.push_back(disjunctionproperty[0]);
                } else
                {
                    DisjunctionStatePredicate *disjucntion = new DisjunctionStatePredicate(
                            disjunctionproperty.size());
                    for (size_t i = 0; i < disjunctionproperty.size(); i++)
                    {
                        disjucntion->addSub(i, disjunctionproperty[i]);
                    }
                    neededProperties.push_back(disjucntion);
                }
                //RT::rep->message("CREATE %d -> %d", neededProperties.size(), curState);
                // increment number of transitions
                bauto->cardTransitions[curState]++;
            }

            bauto->nextstate[curState] = new uint32_t [bauto->cardTransitions[curState]]();
            bauto->guard[curState] = new StatePredicate *[bauto->cardTransitions[curState]]();
            int current_on_trans = -1;
            for (t = s->trans->nxt; t != s->trans; t = t->nxt)
            {
                // bauto data structures
                current_on_trans++;
                bauto->guard[curState][current_on_trans] = neededProperties[curProperty++];
                bauto->nextstate[curState][current_on_trans] =
                        state_id[t->to->final][t->to->id];
            }
        }

        //
        // build a list of all needed propositions
        //

        // if the automata contains an all-accepting state

	RT::data["task"]["buchi"]["states"] = static_cast<int>(bauto->getNumberOfStates());
        RT::rep->status("the resulting Büchi automaton has %d states", bauto->getNumberOfStates());
        if (RT::args.writeBuechi_given)
        {
            RT::rep->status("output: Buechi automaton (%s)",
                    RT::rep->markup(MARKUP_PARAMETER, "--writeBuechi").str());
            bauto->writeBuechi();
        }

    }
    if (RT::args.buechi_given)
    {
        {
            RT::currentInputFile = NULL;
            buechiStateTable = new SymbolTable();

            // Check if the paramter of --buechi is a file that we can open: if that
            // works, parse the file. If not, parse the string.
            FILE *file;
            if ((file = fopen(RT::args.buechi_arg, "r")) == NULL and errno == ENOENT)
            {
                // reset error
                errno = 0;
                ptbuechi__scan_string(RT::args.buechi_arg);
            } else
            {
                fclose(file);
                RT::currentInputFile = new Input("Buechi", RT::args.buechi_arg);
                ptbuechi_in = *RT::currentInputFile;
            }

            //RT::rep->message("Parsing Büchi-Automaton");
            // parse the formula
            ptbuechi_parse();

            //RT::rep->message("Finished Parsing");

            // restructure the formula: unfold complex constructs and handle negations and tautologies
            //TheBuechi = TheBuechi->rewrite(kc::goodbye_doublearrows);
            //TheBuechi = TheBuechi->rewrite(kc::goodbye_singlearrows);
            //TheBuechi = TheBuechi->rewrite(kc::goodbye_xor);
            //TheBuechi = TheBuechi->rewrite(kc::goodbye_initial);

            // restructure the formula: again tautoglies and simplification
            //TheBuechi = TheBuechi->rewrite(kc::sides);
            //TheBuechi = TheBuechi->rewrite(kc::productlists);
            //TheBuechi = TheBuechi->rewrite(kc::leq);
            TheBuechi = TheBuechi->rewrite(kc::tautology);

            // expand the transitions rules
            TheBuechi = TheBuechi->rewrite(kc::rbuechi);

            //RT::rep->message("parsed Buechi");
            //TheBuechi->unparse(myprinter, kc::out);

            //RT::rep->message("checking LTL");

            // prepare counting of place in the formula
            extern bool *place_in_formula;
            extern unsigned int places_mentioned;
            extern unsigned int unique_places_mentioned;
            place_in_formula = new bool[Net::Card[PL]]();
            places_mentioned = 0;
            unique_places_mentioned = 0;

            // copy restructured formula into internal data structures
            TheBuechi->unparse(myprinter, kc::buechi);
            bauto = TheBuechi->automata;
            // XXX: this _must_ work according to the kimwitu docu, but it does not, kimwitu produces memory leaks!
            //TODO: this makes buechi LTL checks segfaulting in some cases ( now leakes
            //memory (we have to take a closer look)
            //TheBuechi->free(true);
            //delete TheBuechi;
            delete buechiStateTable;

            //RT::rep->message("Processed Büchi-Automaton");

            // report places mentioned in the formula and clean up
            RT::rep->status("formula mentions %d of %d places; total mentions: %d",
                    unique_places_mentioned, Net::Card[PL], places_mentioned);
            delete[] place_in_formula;

            // tidy parser
            ptbuechi_lex_destroy();
            //delete TheFormula;

            if (RT::args.buechi_given)
            {
                delete RT::currentInputFile;
                RT::currentInputFile = NULL;
            }

            // reading the buechi automata
            assert(bauto);
	RT::data["task"]["buchi"]["states"] = static_cast<int>(bauto->getNumberOfStates());
        }
    }
    if(RT::args.stubborn_arg != stubborn_arg_off)
    {
	Transition::Visible = new bool[Net::Card[TR]];
	if(TheFormula) TheFormula->unparse(myprinter,kc::visible);
    }
   for (unsigned int i = 0; i < RT::args.jsoninclude_given; ++i)
    {
        if (RT::args.jsoninclude_arg[i] == jsoninclude_arg_net)
        {
                int vis = 0;
                for(arrayindex_t i = 0; i < Net::Card[TR];i++)
                {
                        if(Transition::Visible[i]) vis++;
                }
                RT::data["task"]["search"]["stubborn"]["visible"] = vis;
                break;
        }
    }

    insertBuchiState(bauto->getNumberOfStates());
    // adjust place indices in formula
    for(arrayindex_t i = 0; i < bauto->getNumberOfStates();i++)
    {
	for(arrayindex_t j = 0; j < bauto->cardTransitions[i];j++)
	{
		bauto->guard[i][j]->adjust(Place::CardSignificant-1,Net::Card[PL]-1);
	}
    }

    // prepare task
    RT::data["task"]["search"]["type"] = "product automaton/dfs";
    ltlStore = StoreCreator<LTLPayload>::createStore(number_of_threads);
    ns = NetState::createNetStateFromInitial();

    // Check, if stubborn sets should be used
    // TODO use the following commented code when firelist for LTL is fully
    // implemented. Remove the subsequently if-check of ltlstubborn_arg and 
    // remove ltlstubborn_arg from cmdline.ggo 
    if(RT::args.formula_given)
    {
    switch(RT::args.stubborn_arg)
    {
    case stubborn_arg_off:
	RT::data["task"]["search"]["stubborn"]["type"] = "no";
         fl = new Firelist();
         break;
    default: 
	if(TheFormula->containsNext)
	{
	RT::data["task"]["search"]["stubborn"]["type"] = "no (formula contains X operator)";
		RT::rep->status("Formula contains X operator; stubborn sets not applicable");
		fl = new Firelist();
	}
	else
	{
	   RT::rep->status("using ltl preserving stubborn set method (%s)", RT::rep->markup(MARKUP_PARAMETER, "--stubborn").str());
	RT::data["task"]["search"]["stubborn"]["type"] = "ltl preserving";
           fl = new FirelistStubbornLTLTarjan();
	}
    }
    }
    else
    {
	RT::data["task"]["search"]["stubborn"]["type"] = "no (no formula given)";
	fl = new Firelist();
    }
    RT::rep->indent(-2);
    RT::rep->status("SEARCH");
    RT::rep->indent(2);
    ltlExploration = new LTLExploration();
}

/*!
\post memory for all members is deallocated
 */
LTLTask::~LTLTask()
{
    // quick exit to avoid lengthy destructor calls
#ifndef USE_PERFORMANCE
    //delete ns;
    //delete ltlStore;
    //delete fl;
    //delete ltlExploration;
    //delete bauto;
#endif
}

/*!
This method starts the actual state space exploration and returns the raw
result.

\return the result of the state exploration
\note This result needs to be interpreted by Task::interpreteResult.
 */
ternary_t LTLTask::getResult()
{
    //TODO can we make these assumptions clearer that the asserts are creating
    assert(ns);
    assert(ltlStore);
    assert(bauto);
    assert(ltlExploration);
    assert(fl);

    bool bool_result(false);
    ternary_t result(TERNARY_FALSE);
    bool_result = ltlExploration->checkProperty(*bauto, *ltlStore, *fl, *NetState::createNetStateFromInitial());

    // temporary result transfer, as long as the variable bool_result is needed
    if (bool_result)
    {
        result = TERNARY_TRUE;
    }
    switch(result)
    {
	case TERNARY_TRUE: result = TERNARY_FALSE; break;
	case TERNARY_FALSE: result = TERNARY_TRUE; break;
	case TERNARY_UNKNOWN: result = TERNARY_UNKNOWN; break;
	default: break;
    }

    return result;
}

/*!
\post The result is interpreted and printed using Reporter::message
\warning This method must not be called more than once.

\todo This method should be internal and automatically be called by
Task::getResult after the result was calculated. Then, a member of type
trinary_t can be displayed.
 */
void LTLTask::interpreteResult(ternary_t result)
{
    switch (result)
    {
        case TERNARY_TRUE:
            RT::rep->status("result: %s", RT::rep->markup(MARKUP_GOOD, "yes").str());
            RT::rep->status("produced by: %s", taskname);
            RT::data["result"]["value"] = true;
            RT::data["result"]["produced_by"] = std::string(taskname);
	
            RT::rep->status("%s", RT::rep->markup(MARKUP_GOOD, "The net satisfies the given formula (language of the product automaton is empty).").str());
            break;

        case TERNARY_FALSE:
            RT::rep->status("result: %s", RT::rep->markup(MARKUP_BAD, "no").str());
            RT::rep->status("produced by: %s", taskname);
            RT::data["result"]["value"] = false;
            RT::data["result"]["produced_by"] = std::string(taskname);

            RT::rep->status("%s", RT::rep->markup(MARKUP_BAD,
                    "The net does not satisfy the given formula (language of the product automaton is nonempty).").str());
            break;

        case TERNARY_UNKNOWN:
            RT::rep->status("result: %s", RT::rep->markup(MARKUP_WARNING, "unknown").str());
            RT::rep->status("produced by: %s", taskname);
            RT::data["result"]["value"] = JSON::null;
            RT::data["result"]["produced_by"] = std::string(taskname);

            RT::rep->status("%s", RT::rep->markup(MARKUP_WARNING,
                    "The net may or may not satisfy the given formula.").str());
            break;
    }
}

Path LTLTask::getWitnessPath()
{
    return *(ltlExploration->path);
}

capacity_t *LTLTask::getMarking()
{
    return NULL;
}

void LTLTask::getStatistics()
{
    uint64_t markings = 0;
    markings = ltlStore->get_number_of_markings();
    RT::data["result"]["markings"] = static_cast<int> (markings);
    uint64_t edges = 0;
    edges = ltlStore->get_number_of_calls();
    RT::data["result"]["edges"] = static_cast<int> (edges);

    RT::rep->status("%llu markings, %llu edges", markings, edges);

}

char * LTLTask::getStatus(uint64_t elapsed)
{
    char * result = new char[STATUSLENGTH];
    uint64_t m = ltlStore -> get_number_of_markings();
    sprintf(result, "%10llu markings, %10llu edges, %8.0f markings/sec, %5llu secs", m, ltlStore->get_number_of_calls(), ((m - previousNrOfMarkings) / (float) REPORT_FREQUENCY), elapsed);
    previousNrOfMarkings = m;
    return result;

}

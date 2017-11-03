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

#include <config.h>
#include <Core/Dimensions.h>
#include <Exploration/CTLExploration.h>
#include <Exploration/FirelistStubbornCTL.h>
#include <Symmetry/Symmetry.h>
#include <Symmetry/Constraints.h>
#include <Exploration/Firelist.h>
#include <Frontend/Parser/ast-system-k.h>
#include <Frontend/Parser/ast-system-rk.h>
#include <Frontend/Parser/ast-system-unpk.h>
#include <Frontend/SymbolTable/SymbolTable.h>
#include <InputOutput/InputOutput.h>
#include <Planning/CTLTask.h>
#include <Planning/StoreCreator.h>
#include <Stores/Store.h>
#include <Witness/Path.h>
#include <Net/NetState.h>

// input files
extern FILE *ptformula_in;

// Kimwitu++ objects
extern kc::tFormula TheFormula;

CTLTask::CTLTask()
{
	previousNrOfMarkings  = 0;
	taskname = "CTL model checker";
        RT::data["task"]["search"]["type"] = "ctl model checker";
	//Task::outputFormula(TheFormula);
    goStatus = false;
    ns = NetState::createNetStateFromInitial();
    // prepare counting of place in the formula
    extern bool *place_in_formula;
    extern unsigned int places_mentioned;
    extern unsigned int unique_places_mentioned;
    place_in_formula = new bool[Net::Card[PL]]();
    places_mentioned = 0;
    unique_places_mentioned = 0;
        // replace path quantor+temporal operator by dedicated CTL operator
        TheFormula = TheFormula->rewrite(kc::tautology);
        TheFormula = TheFormula->rewrite(kc::ctloperators);
        TheFormula = TheFormula->rewrite(kc::tautology);
        TheFormula->unparse(myprinter, kc::ctl);
	assert(TheFormula);
	assert(TheFormula->ctl_formula);
        ctlFormula = TheFormula->ctl_formula;
	Task::outputFormulaAsProcessed();

        assert(ctlFormula);
    // prepare task
    ctlStore = StoreCreator<void *>::createStore(number_of_threads);
     if(RT::args.stubborn_arg != stubborn_arg_off)
     {
	// use stubborn sets
	
	// mark visible transitions
	Transition::Visible = new bool [Net::Card[TR]];
	TheFormula->unparse(myprinter,kc::visible);
	
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
	if(TheFormula->containsNext)
	{
		// application of stubborn sets not possible
		RT::rep->status("Formula contains EX or AX operators, stubborn sets not applicable");
		fl = new Firelist();
                RT::data["task"]["search"]["stubborn"]["type"] = "no (formula contains X operator)";
	}
	else
	{
		// compute conflict clusters
		Net::computeConflictClusters();

		// use stubborn set firelist generator
		fl = new FirelistStubbornCTL();
		RT::rep->status("Using CTL preserving stubborn sets");
                RT::data["task"]["search"]["stubborn"]["type"] = "ctl preserving";
	}
     }
     else
     {
            RT::data["task"]["search"]["stubborn"]["type"] = "no";
	    fl = new Firelist();
     }
    ctlExploration = new CTLExploration();
}


/*!
\post memory for all members is deallocated
*/
CTLTask::~CTLTask()
{
#ifndef USE_PERFORMANCE
    delete ns;
    delete ctlStore;
    delete fl;
    delete ctlExploration;
#endif
}


ternary_t CTLTask::getResult()
{
    //TODO can we make these assumptions clearer that the asserts are creating
    assert(ns);
    assert(ctlStore); 
    assert(ctlFormula);
    assert(ctlExploration);
    assert(fl);

    // compute symmetries
    if (RT::args.symmetry_given)
    {
        SymmetryCalculator *SC = new SymmetryCalculator(ctlFormula);
        assert(SC);
        SC->ComputeSymmetries();
        delete SC;
	ctlStore = reinterpret_cast<SymmetryStore<void*>*>(ctlStore)->setGeneratingSet(SymmetryCalculator::G);
    }
    goStatus = true;

    bool bool_result(false);
    ternary_t result(TERNARY_FALSE);
    bool_result = ctlExploration->checkProperty(ctlFormula, *ctlStore, *fl, *ns);
    if (bool_result)
    {
        result = TERNARY_TRUE;
    }

    return result;
}


void CTLTask::interpreteResult(ternary_t result)
{
    switch (result)
    {
    case TERNARY_TRUE:
        RT::rep->status("result: %s", RT::rep->markup(MARKUP_GOOD, "yes").str());
        RT::rep->status("produced by: %s", taskname);
        RT::data["result"]["produced_by"] = std::string(taskname);
        RT::data["result"]["value"] = true;
        RT::rep->status("%s", RT::rep->markup(MARKUP_GOOD, "The net satisfies the given formula.").str());

        break;

    case TERNARY_FALSE:
        RT::rep->status("result: %s", RT::rep->markup(MARKUP_BAD, "no").str());
        RT::rep->status("produced by: %s", taskname);
        RT::data["result"]["produced_by"] = std::string(taskname);
        RT::data["result"]["value"] = false;
            RT::rep->status("%s", RT::rep->markup(MARKUP_BAD,
                                                  "The net does not satisfy the given formula.").str());

        break;

    case TERNARY_UNKNOWN:
        RT::rep->status("result: %s", RT::rep->markup(MARKUP_WARNING, "unknown").str());
        RT::rep->status("produced by: %s", taskname);
        RT::data["result"]["produced_by"] = std::string(taskname);
        RT::data["result"]["value"] = JSON::null;
            RT::rep->status("%s", RT::rep->markup(MARKUP_WARNING,
                                                  "The net may or may not satisfy the given formula.").str());

        break;
    }
}


Path CTLTask::getWitnessPath() 
{
        return ctlExploration->path();
}


capacity_t *CTLTask::getMarking() 
{
	return NULL;
}

void CTLTask::getStatistics()
{
        uint64_t result = ctlStore->get_number_of_calls();
        uint64_t result2 = ctlStore->get_number_of_markings();

            RT::rep->status("%llu markings, %llu edges", result2, result);
    RT::data["result"]["markings"] = static_cast<int>(result2);
    RT::data["result"]["edges"] = static_cast<int>(result);

}

char * CTLTask::getStatus(uint64_t elapsed)
{
	if(!goStatus)
	{
		return NULL;
	}
	char * result = new char[STATUSLENGTH];
	uint64_t m = ctlStore -> get_number_of_markings();
	sprintf(result,"%10llu markings, %10llu edges, %8.0f markings/sec, %5llu secs", m, ctlStore->get_number_of_calls(), ((m - previousNrOfMarkings) / (float)REPORT_FREQUENCY), elapsed);
	previousNrOfMarkings = m;
	return result;

}

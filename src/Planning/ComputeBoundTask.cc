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


#include <Frontend/Parser/ast-system-k.h>
#include <Frontend/Parser/ast-system-rk.h>
#include <Frontend/Parser/ast-system-unpk.h>
#include <CoverGraph/CoverPayload.h>
#include <Symmetry/Constraints.h>
#include <SweepLine/Sweep.h>
#include <Core/Dimensions.h>
#include <Core/Runtime.h>
#include <Core/Handlers.h>
#include <Witness/Path.h>
#include <Planning/Task.h>
#include <Planning/StoreCreator.h>
#include <Planning/ComputeBoundTask.h>
#include <Exploration/StatePredicateProperty.h>
#include <Exploration/FirelistStubbornComputeBound.h>
#include <Exploration/FirelistStubbornComputeBoundCombined.h>
#include <Exploration/ComputeBoundExploration.h>



/*!
\brief the verification task

This class wraps the reachability check by statespace exploration

*/

extern kc::tFormula TheFormula;

ComputeBoundTask::ComputeBoundTask()
{
	RT::data["task"]["workflow"] = "search";
	taskname = "state space";
	goStatus = false;
        // extract state predicate from formula
	assert(TheFormula);

        TheFormula->unparse(myprinter, kc::internal);
    Task::outputFormulaAsProcessed();
        spFormula = TheFormula->formula;
	previousNrOfMarkings = 0;
	// set the net
    ns = NetState::createNetStateFromInitial();

        RT::data["task"]["search"]["type"] = "dfs";

        // choose a store
	store = StoreCreator<void>::createStore(1); // 1 = nr_of_threads
	covStore =NULL;
    // choose a simple property
        p = new StatePredicateProperty(spFormula);
	RT::rep->indent(-2);
	RT::rep->status("SEARCH");
            switch(RT::args.stubborn_arg)
            {
	    case stubborn_arg_off:
RT::rep->status("not using stubborn set method (%s)", RT::rep->markup(MARKUP_PARAMETER, "--stubborn=off").str());
		fl = new Firelist();
	RT::data["task"]["search"]["stubborn"]["type"] = "no";
		break;
	   case stubborn_arg_tarjan:
RT::rep->status("using bound preserving stubborn set method with insertion algorithm(%s)", RT::rep->markup(MARKUP_PARAMETER, "--stubborn=tarjan").str());
                fl = new FirelistStubbornComputeBound(spFormula);
	RT::data["task"]["search"]["stubborn"]["type"] = "bound preserving/insertion";
		break;
	    default: 
	RT::data["task"]["search"]["stubborn"]["type"] = "bound preserving/combined";
		RT::rep->status("using bound preserving stubborn set method with combined insertion/deletion algorithm(%s)", RT::rep->markup(MARKUP_PARAMETER, "--stubborn=combined").str());
		fl = new FirelistStubbornComputeBoundCombined(spFormula);
            ; 
            }

                exploration = new ComputeBoundExploration();
}

ComputeBoundTask::~ComputeBoundTask()
{
#ifndef USE_PERFORMANCE
    delete ns;
    delete store;
    delete covStore;
    delete p;
    delete spFormula;
    delete fl;
    delete exploration;
#endif
}

ternary_t ComputeBoundTask::getResult()
{
	// compute symmetries
    if (RT::args.symmetry_given)
    {
        SymmetryCalculator *SC = NULL;
	SC = new SymmetryCalculator(spFormula);
        assert(SC);
        SC->ComputeSymmetries();
	if(store) store = reinterpret_cast<SymmetryStore<void> *>(store)->setGeneratingSet(SymmetryCalculator::G);
	if(covStore) covStore = reinterpret_cast<SymmetryStore<CoverPayload> *>(covStore)->setGeneratingSet(SymmetryCalculator::G);
        delete SC;
    }
    goStatus = true;

	    resultvalue = reinterpret_cast<ComputeBoundExploration*>(exploration)->depth_first_num(*p, *ns, *store, *fl, 1); // 1 = nr_of_threads
    result = TERNARY_FALSE;
    return result;
}

void ComputeBoundTask::interpreteResult(ternary_t result)
{
	if (RT::args.store_arg == store_arg_bloom)
    {
        double n;
        if(store)
        {
        	n = static_cast<double>(store->get_number_of_markings());
	}
	else
	{
		assert(covStore);
        	n = static_cast<double>(covStore->get_number_of_markings());
		
	}
        const double k = RT::args.hashfunctions_arg;
        const double m = static_cast<double>(BLOOM_FILTER_SIZE);
        const double prob = pow((1.0 - exp((-k * n) / m)), k);
        RT::rep->status("Bloom filter: probability of false positive is %.10lf",
                        prob);
        const double opt_hash = log(m / n) / log(2.0);
        RT::rep->status("Bloom filter: optimal number of hash functions is %.1f",
                        opt_hash);
	RT::data["task"]["search"]["bloom"]["prob_false_positive"] = prob;
	RT::data["task"]["search"]["bloom"]["optimal_hash_functions"] = opt_hash;
    }
    // Bloom store result is not reliable
    if (RT::args.store_arg == store_arg_bloom)
    {
                result = TERNARY_UNKNOWN;
    }
	RT::data["result"]["value"] = static_cast<int>(resultvalue);
	RT::data["result"]["produced_by"] = "search";
	char * value = new char[1000];
    switch(result)
    {
    case TERNARY_FALSE:
        sprintf(value,"%lld",resultvalue);
        RT::rep->status("result: %s", RT::rep->markup(MARKUP_GOOD, value).str());
        RT::rep->status("produced by: %s", taskname);
       	
        RT::data["result"]["value"] = (int)resultvalue;
	RT::data["result"]["produced_by"] = "search";
        sprintf(value,"The maximum value of the given expression is %lld",resultvalue);
            RT::rep->status("%s", RT::rep->markup(MARKUP_GOOD, value).str());
        break;

    case TERNARY_UNKNOWN:
        sprintf(value,"%lld",resultvalue);
        RT::rep->status("result: %s", RT::rep->markup(MARKUP_WARNING, value).str());
        RT::rep->status("produced by: %s", taskname);
        RT::data["result"]["value"] = (int)(resultvalue);
	RT::data["result"]["produced_by"] = "search";
        sprintf(value,"The maximum value of the given expression is at least %lld",resultvalue);
            RT::rep->status("%s", RT::rep->markup(MARKUP_WARNING, value).str());
        break;

    default: assert(false);
    }

}
 
void ComputeBoundTask::getStatistics()
{
	uint64_t markingcount;
	if(store)
	{
		markingcount = store->get_number_of_markings();
	}
	else if(covStore)
	{
		markingcount = covStore->get_number_of_markings();
	}
	else
	{
		assert(false);
	}
	RT::data["result"]["markings"] = static_cast<int>(markingcount);
	uint64_t edgecount;
    if (store)
    {
        edgecount = store->get_number_of_calls();
    }
	else if (covStore)
    {
        edgecount = covStore->get_number_of_calls();
    }
    else
    {
	assert(false);
    }
    RT::data["result"]["edges"] = static_cast<int>(edgecount);
    RT::rep->status("%llu markings, %llu edges", markingcount, edgecount);
} 

Task * ComputeBoundTask::buildTask()
{
	return new ComputeBoundTask();
}


char * ComputeBoundTask::getStatus(uint64_t elapsed)
{
	if(!goStatus)
	{
		return NULL;
	}
        char * result = new char[STATUSLENGTH];
        uint64_t m = store->get_number_of_markings();
        sprintf(result,"%10llu markings, %10llu edges, %8.0f markings/sec, %5llu secs", m, store->get_number_of_calls(), ((m - previousNrOfMarkings) / (float)REPORT_FREQUENCY), elapsed); 
	previousNrOfMarkings = m;
            // report probability of a false positive in the Bloom filter
            if (RT::args.store_arg == store_arg_bloom)
            {
                const double n = static_cast<double>(m);
                static const double k = static_cast<double>(RT::args.hashfunctions_arg);
                static const double mm = static_cast<double>(BLOOM_FILTER_SIZE);
                sprintf(result+strlen(result),"%10lu hash table size      false positive probability: %.10lf", BLOOM_FILTER_SIZE, pow((1.0 - exp((-k * n) / mm)), k));
            }
        return result;

}



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
#include <Frontend/Parser/ast-system-k.h>
#include <Frontend/Parser/ast-system-rk.h>
#include <Frontend/Parser/ast-system-unpk.h>
#include <InputOutput/InputOutput.h>
#include <Net/NetState.h>
#include <Planning/Task.h>
#include <Planning/FullTask.h>
#include <Planning/BooleanTask.h>
#include <Planning/ComputeBoundTask.h>
#include <Planning/CompoundTask.h>
#include <Planning/LTLTask.h>
#include <Planning/CTLTask.h>
#include <Planning/DeadlockTask.h>
#include <Planning/NoDeadlockTask.h>
#include <Planning/AGEFAGTask.h>
#include <Planning/EFAGTask.h>
#include <Planning/ReachabilityTask.h>
#include <Planning/InvariantTask.h>
#include <Planning/InitialTask.h>
#include <Planning/EmptyTask.h>
#include <Witness/Path.h>
#include <Stores/NetStateEncoder/BitEncoder.h>
#include <Stores/NetStateEncoder/CopyEncoder.h>
#include <Stores/NetStateEncoder/FullCopyEncoder.h>
#include <Stores/NetStateEncoder/SimpleCompressedEncoder.h>
#include <Frontend/SymbolTable/SymbolTable.h>
#include <Formula/StatePredicate/MagicNumber.h>

#ifdef RERS
	bool * rers_place = new bool [10000000];
	void create_rers_output();
#endif
// the parsers
extern int ptformula_parse();
extern int ptformula_lex_destroy();
extern int ptbuechi_parse();
extern int ptbuechi_lex_destroy();

// input files
extern FILE *ptformula_in;
extern FILE *ptbuechi_in;

// code to parse from a string
struct yy_buffer_state;
typedef yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE ptformula__scan_string(const char *yy_str);
extern YY_BUFFER_STATE ptbuechi__scan_string(const char *yy_str);

extern SymbolTable *buechiStateTable;
extern FILE *tl_out;

threadid_t Task::number_of_threads;

// Kimwitu++ objects
extern kc::tFormula TheFormula;
extern kc::tBuechiAutomata TheBuechi;

/// process formula and check cmdline parameters
/// to dispatch to the right task object.

Task * Task::buildTask() 
{
    Task * result = NULL;
    // prepare counting of place in the formula
    extern bool *place_in_formula;
    extern unsigned int places_mentioned;
    extern unsigned int unique_places_mentioned;
    place_in_formula = new bool[Net::Card[PL]]();
    places_mentioned = 0;
    unique_places_mentioned = 0;

    if (RT::args.markinglimit_arg != 0)
    {
        RT::data["call"]["markinglimit"] = static_cast<int>(RT::args.markinglimit_arg);
    }
    else
    {
        RT::data["call"]["markinglimit"] = JSON::null;
    }

    number_of_threads = static_cast<threadid_t>(RT::args.threads_arg);

    if (RT::args.check_arg == check_arg_none || RT::args.check_arg == check__NULL)
    {
	RT::rep->status("checking nothing (%s)",
	RT::rep->markup(MARKUP_PARAMETER, "--check=none").str());
        RT::data["task"]["type"] = "none";
	return EmptyTask::buildTask();
    }
    if (RT::args.check_arg == check_arg_full)
    {
	RT::rep->status("creating the whole state space (%s)",
	RT::rep->markup(MARKUP_PARAMETER, "--check=full").str());
        RT::data["task"]["type"] = "full";
	return FullTask::buildTask();
    }

    if (RT::args.check_arg == check_arg_modelchecking)
    {
	// model checking requires a property whch must be given either
	// as formula or as buchi automaton
        if ((not RT::args.formula_given) and (not RT::args.buechi_given))
        {
            RT::rep->message("%s given without %s or %s",
	     RT::rep->markup(MARKUP_PARAMETER, "--check=modelchecking").str(),
	     RT::rep->markup(MARKUP_PARAMETER, "--formula").str(),
	     RT::rep->markup(MARKUP_PARAMETER, "--buechi").str());
            RT::rep->abort(ERROR_COMMANDLINE);
        }
        if (RT::args.formula_given and RT::args.buechi_given)
        {
            RT::rep->message("both %s and %s given",
	     RT::rep->markup(MARKUP_PARAMETER, "--formula").str(),
	     RT::rep->markup(MARKUP_PARAMETER, "--buechi").str());
            RT::rep->abort(ERROR_COMMANDLINE);
        }
    }

    // process the formula according to the type
    if (RT::args.buechi_given)
    {
	RT::rep->status("checking LTL");
        RT::data["task"]["type"] = "LTL";
	return LTLTask::buildTask();
    }

    // process formula
    assert(RT::args.formula_given);

    MagicNumber::nextMagicNumber = MAGIC_NUMBER_INITIAL;
    parseFormula();
    Task::outputFormula(TheFormula);
	    
#ifdef RERS
	//create_rers_output();
#endif
    // fork off the special case that we deal with a compound formula

    TheFormula->unparse(myprinter, kc::detectcompound);

    if(TheFormula->type == FORMULA_COMPOUND)
    {
	RT::rep->status("computing a collection of formulas");
        RT::data["task"]["type"] = "compound";
        result = CompoundTask::buildTask(); 
    ptformula_lex_destroy();
	return result;
    }
    setFormula();
	
    switch (TheFormula->type)
    {
    case FORMULA_BOUND:
	RT::rep->status("computing the bound for an expression");
        RT::data["task"]["type"] = "bound";
        result = ComputeBoundTask::buildTask(); 
	break;
    case FORMULA_DEADLOCK:
	RT::rep->status("checking reachability of deadlocks");
        RT::data["task"]["type"] = "deadlock";
        result = DeadlockTask::buildTask(); 
	break;
    case FORMULA_NODEADLOCK:
	RT::rep->status("checking absence of deadlocks");
        RT::data["task"]["type"] = "nodeadlock";
        result = NoDeadlockTask::buildTask(); 
	break;
    case FORMULA_REACHABLE:
	RT::rep->status("checking reachability");
        RT::data["task"]["type"] = "reachability";
	result = ReachabilityTask::buildTask(); 
	break;
    case FORMULA_INVARIANT:
	RT::rep->status("checking invariance");
        RT::data["task"]["type"] = "invariance";
	result = InvariantTask::buildTask(); 
	break;
    case FORMULA_LIVENESS:
	RT::rep->status("checking liveness");
        RT::data["task"]["type"] = "liveness";
	result = AGEFTask::buildTask(); 
        //RT::rep->status("liveness not yet implemented, converting to CTL...");
	//result = CTLTask::buildTask(); 
	break;
    case FORMULA_EGAGEF:
	RT::rep->status("checking possible liveness");
        RT::data["task"]["type"] = "possible_liveness";
	result = EFAGEFTask::buildTask(); 
        //RT::rep->status("possible liveness not yet implemented, converting to CTL...");
	//result = CTLTask::buildTask(); 
	break;
    case FORMULA_EFAG:
	RT::rep->status("checking possible invariance");
        RT::data["task"]["type"] = "possible_invariance";
	result = EFAGTask::buildTask(); 
        //RT::rep->status("possible invariance not yet implemented, converting to CTL...");
	//result = CTLTask::buildTask(); 
	break;
    case FORMULA_AGEFAG:
	RT::rep->status("checking globally possible invariance");
        RT::data["task"]["type"] = "globally_possible_invariance";
	result = AGEFAGTask::buildTask(); 
        //RT::rep->status("globally possible invariance not yet implemented, converting to CTL...");
	//result = CTLTask::buildTask(); 
	break;
    case FORMULA_FAIRNESS:
        RT::rep->status("checking fairness");
        RT::data["task"]["type"] = "fairness";
        RT::rep->status("fairness not yet implemented, converting to LTL...");
	result = LTLTask::buildTask(); 
	break;
    case FORMULA_STABILIZATION:
        RT::rep->status("checking stabilization");
        RT::data["task"]["type"] = "stabilization";
        RT::rep->status("stabilization not yet implemented, converting to LTL...");
	result = LTLTask::buildTask(); 
	break;
    case FORMULA_EVENTUALLY:
        RT::rep->status("checking eventual occurrence");
        RT::data["task"]["type"] = "eventual occurrence";
	RT::rep->status("eventual occurrence not yet implemented, converting to LTL...");
	result = LTLTask::buildTask(); 
	break;
    case FORMULA_INITIAL:
	RT::rep->status("checking initial satisfaction");
        RT::data["task"]["type"] = "initial satisfaction";
	result = InitialTask::buildTask(); 
	break;
    case FORMULA_BOTH:
	if(RT::args.preference_arg == preference_arg_ctl)
	{
		RT::rep->status("checking CTL (%s)",
		RT::rep->markup(MARKUP_PARAMETER, "--preference=ctl").str());
		RT::data["task"]["type"] = "CTL";
		result = CTLTask::buildTask();
	}
	else
	{
		RT::rep->status("checking LTL");
		RT::data["task"]["type"] = "LTL";
		result = LTLTask::buildTask();
	}
	break;
    case FORMULA_LTL:
	RT::rep->status("checking LTL");
        RT::data["task"]["type"] = "LTL";
	result = LTLTask::buildTask();
	break;
    case FORMULA_CTL:
	RT::rep->status("checking CTL");
        RT::data["task"]["type"] = "CTL";
        result = CTLTask::buildTask(); 
	break;
    case FORMULA_BOOLEAN:
	RT::rep->status("checking a Boolean combination of subproblems");
        RT::data["task"]["type"] = "Boolean";
        result = BooleanTask::buildTask(); 
	break;
    case FORMULA_MODELCHECKING:
        RT::rep->status("checking CTL*");
        RT::data["task"]["type"] = "CTL*";
        RT::rep->message("check not yet implemented");
        RT::rep->abort(ERROR_COMMANDLINE);
    default: assert(false); // complete case consideration
    }
    // tidy parser
    ptformula_lex_destroy();

    return result;
}

void Task::parseFormula()
{

    RT::currentInputFile = NULL;

    // Check if the paramter of --formula is a file that we can open: if that
    // works, parse the file. If not, parse the string.
    FILE *file = fopen(RT::args.formula_arg, "r");
    if (file == NULL and (errno == ENOENT or errno == ENAMETOOLONG))
    {
        // reset error
        errno = 0;
        ptformula__scan_string(RT::args.formula_arg);
    }
    else
    {
        fclose(file);
        RT::currentInputFile = new Input("formula", RT::args.formula_arg);
        ptformula_in = *RT::currentInputFile;
        // Save formula input file name for sara problem file name
        RT::inputFormulaFileName = RT::currentInputFile->getFilename();
    }

    // parse the formula
    ptformula_parse();
    TheFormula->unparse(myprinter, kc::elem);
    for (unsigned int i = 0; i < RT::args.jsoninclude_given; ++i)
    {
        if (RT::args.jsoninclude_arg[i] == jsoninclude_arg_formula)
        {
	    unparsed.clear();
            TheFormula->unparse(stringprinter, kc::out);
	    RT::data["formula"]["read"] = unparsed;
	    RT::data["formula"]["read_size"] = static_cast<int>(unparsed.size());
            break;

        }
    }
}

void Task::setFormula()
{
    // restructure the formula:

    // Phase 1: remove syntactic sugar
    TheFormula = TheFormula->rewrite(kc::goodbye_doublearrows);
    TheFormula = TheFormula->rewrite(kc::goodbye_singlearrows);
    TheFormula = TheFormula->rewrite(kc::goodbye_xor);
    //TheFormula = TheFormula->rewrite(kc::goodbye_initial);

    // Phase 2: Normalize atomic propositions
    // - places to left, constants to right
    //TheFormula = TheFormula->rewrite(kc::sides);
    // - left side to lists
    //TheFormula = TheFormula->rewrite(kc::productlists);
    // - remove == != >= < (<= and > remain)
    //TheFormula = TheFormula->rewrite(kc::leq);

    // Phase 3: Apply logical tautologies.
	Task::outputFormula(TheFormula);
    TheFormula = TheFormula->rewrite(kc::tautology);

    // Phase 3a: Remove empty path quantifiers (containsTemporal is set in kc::temporal)
    TheFormula->unparse(myprinter, kc::temporal);
    TheFormula = TheFormula->rewrite(kc::emptyquantifiers);

    // 4a: detect formula type
    TheFormula->unparse(myprinter, kc::temporal);
}


void Task::outputFormulaAsProcessed()
{
    unparsed.clear();
    TheFormula->unparse(stringprinter, kc::out);
    if (unparsed.size() < FORMULA_PRINT_SIZE)
    {
        RT::rep->status("processed formula: %s", unparsed.c_str());
    }
    else
    {
        RT::rep->status("processed formula: %s... (shortened)", unparsed.substr(0,
                        FORMULA_PRINT_SIZE).c_str());
    }
    RT::rep->status("processed formula length: %d", unparsed.size());
    RT::data["formula"]["processed_size"] = static_cast<int>(unparsed.size());
    RT::rep->status("%d rewrites", rule_applications);
    RT::data["formula"]["rewrites"] = static_cast<int>(rule_applications);
    for (unsigned int i = 0; i < RT::args.jsoninclude_given; ++i)
    {
        if (RT::args.jsoninclude_arg[i] == jsoninclude_arg_formula)
        {
     	    RT::data["formula"]["processed"] = unparsed;
            break;

        }
    }

    for (unsigned int i = 0; i < RT::args.jsoninclude_given; ++i)
    {
        if (RT::args.jsoninclude_arg[i] == jsoninclude_arg_formulastat)
        {
	    FormulaStatistics * fs = new FormulaStatistics();
	    TheFormula->fs = fs;
	    TheFormula->unparse(myprinter,kc::count);
	    fs = TheFormula->fs;
	    RT::data["formula"]["count"]["A"] = fs->A;
	    RT::data["formula"]["count"]["E"] = fs->E;
	    RT::data["formula"]["count"]["F"] = fs->F;
	    RT::data["formula"]["count"]["G"] = fs->G;
	    RT::data["formula"]["count"]["X"] = fs->X;
	    RT::data["formula"]["count"]["U"] = fs->U;
	    RT::data["formula"]["count"]["tconj"] = fs->tconj;
	    RT::data["formula"]["count"]["tdisj"] = fs->tdisj;
	    RT::data["formula"]["count"]["tneg"] = fs->tneg;
	    RT::data["formula"]["count"]["taut"] = fs->taut;
	    RT::data["formula"]["count"]["cont"] = fs->cont;
	    RT::data["formula"]["count"]["dl"] = fs->dl;
	    RT::data["formula"]["count"]["nodl"] = fs->nodl;
	    RT::data["formula"]["count"]["fir"] = fs->fir;
	    RT::data["formula"]["count"]["unfir"] = fs->unfir;
	    RT::data["formula"]["count"]["comp"] = fs->comp;
	    RT::data["formula"]["count"]["aconj"] = fs->aconj;
	    RT::data["formula"]["count"]["adisj"] = fs->adisj;
	    RT::data["formula"]["count"]["visible_places"] = fs->visible_places;
	    RT::data["formula"]["count"]["visible_transitions"] = fs->visible_transitions;
	    RT::data["formula"]["count"]["place_references"] = fs->place_references;
	    RT::data["formula"]["count"]["transition_references"] = fs->transition_references;
	    RT::data["formula"]["count"]["aneg"] = fs->aneg;
            break;
        }
    }

	
    if (RT::args.formula_given)
    {
        delete RT::currentInputFile;
        RT::currentInputFile = NULL;
    }
}


void Task::outputFormula(void  * V)
{
	tFormula F = (tFormula) V;
    unparsed.clear();
    F->unparse(stringprinter, kc::out);
    RT::rep->status("%s", unparsed.c_str());
    // save already computed atomic formulas in leaf node of parse tree
}

#ifdef RERS
void create_rers_output()
{
	bool rers_trans[10000000];
	for(arrayindex_t i = 0; i < 10000000; i++)
	{
		rers_trans[i] = false;
	}
#include <iostream>
#include <fstream>
    static std::string NetFileName = RT::inputFormulaFileName;
        const size_t period_idx = NetFileName.rfind('.');
        if (std::string::npos != period_idx)
        {
            NetFileName.erase(period_idx);
        }
	NetFileName = NetFileName + ".rerslola";

    std::ofstream netfile(NetFileName.c_str(), std::ios::out);

     netfile << "PLACE SAFE :" << std:: endl;
	for(arrayindex_t i = 0; i < Net::Card[PL];i++)
	{
		if(Net::Name[PL][i][0] != 'l' || 
		   Net::Name[PL][i][1] != 'a' ||
		   rers_place[i])
		{
		   
			netfile << Net::Name[PL][i] << "," << std::endl;
		}
		if(Net::Name[PL][i][0] == 'l' &&
                   Net::Name[PL][i][1] == 'a' &&
                   rers_place[i])
		{
			rers_trans[Net::Arc[PL][POST][i][0]] = true;
		}
	}
	for(arrayindex_t i = 0; i < Net::Card[TR];i++)
	{
		if(rers_trans[i])
		{
			netfile << "firing_" << Net::Name[TR][i] << "," << std::endl;
		}
	}
	netfile << "mutex ; " << std::endl << "MARKING" << std::endl;
	for(arrayindex_t i = 0; i < Net::Card[PL];i++)
	{
		if(Marking::Initial[i])
		{
			netfile << Net::Name[PL][i] << " : 1," << std::endl;
		}
	}
	netfile << "mutex : 1;" << std::endl;
	for(arrayindex_t i = 0; i < Net::Card[TR];i++)
	{
		std::string trname = Net::Name[TR][i];
//		int pos = trname.find_first_of("_");
//		pos = trname.find_first_of("_",pos+1);
//		trname = trname.substr(pos+1);
		if(rers_trans[i])
		{
			const char * name;
			netfile << "TRANSITION start_" << trname << std::endl << "CONSUME" << std::endl;
			for(arrayindex_t j = 0; j < Net::CardArcs[TR][PRE][i];j++)
			{
				netfile << Net::Name[PL][Net::Arc[TR][PRE][i][j]] << " : 1";
				netfile << "," << std::endl;
			}
			netfile << "mutex : 1 ; " << std::endl;
			netfile << "PRODUCE" << std::endl;
			netfile << "firing_" << trname << " : 1 ," <<  std::endl;
			for(arrayindex_t k = 0; k < Net::CardArcs[TR][POST][i];k++)
			{
				name = Net::Name[PL][Net::Arc[TR][POST][i][k]];
				if(name[0] == 'l' && name[1] == 'a') break;
			}
			netfile  << name << " : 1 ; "<< std::endl << std::endl;
			netfile << "TRANSITION end_" << trname << std::endl; 
			netfile << "CONSUME" << std::endl;
			netfile << "firing_" << Net::Name[TR][i] << " : 1 ," <<  std::endl ;
			netfile << name << " : 1 ; " << std::endl;
			netfile << "PRODUCE" << std::endl;
			for(arrayindex_t j = 0; j < Net::CardArcs[TR][POST][i];j++)
			{
				netfile << Net::Name[PL][Net::Arc[TR][POST][i][j]] << " : 1";
				netfile << "," << std::endl;
			}
			netfile << "mutex : 1 ;" << std::endl;
		}
		else
		{
			const char * name;
			netfile << "TRANSITION " << trname << std::endl; 
			netfile << "CONSUME" << std::endl;
			bool needcomma = 0;
			for(arrayindex_t j = 0; j < Net::CardArcs[TR][PRE][i];j++)
			{
				name = Net::Name[PL][Net::Arc[TR][PRE][i][j]];
				if(name[0] != 'l' && name[1] != 'a')
				{	
					if(needcomma)
					{
						netfile << " ," << std::endl;
					}
					needcomma = true;
					netfile << name << " : 1";
				}
			}
			needcomma = false;
			netfile << ";" << std::endl;
			netfile << "PRODUCE" << std::endl;
			for(arrayindex_t j = 0; j < Net::CardArcs[TR][POST][i];j++)
			{
				name = Net::Name[PL][Net::Arc[TR][POST][i][j]];
				if(name[0] != 'l' && name[1] != 'a')
				{
					if(needcomma)
					{
						netfile << " ," << std::endl;
					}
					needcomma = true;
					netfile << name << " : 1";
				
				}
			}
			netfile << ";" << std::endl << std::endl;
		}
	}
	netfile.close();
}
#endif

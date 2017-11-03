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

#include <Core/Runtime.h>
#include <Planning/Task.h>
#include <Planning/StateEquationTask.h>
#include <Frontend/Parser/ast-system-k.h>
#include <Frontend/Parser/ast-system-rk.h>
#include <Frontend/Parser/ast-system-unpk.h>
#include<Formula/StatePredicate/AtomicBooleanPredicate.h>
#include<Formula/StatePredicate/AtomicStatePredicate.h>
#include<Formula/StatePredicate/MagicNumber.h>
#include <Exploration/DFSExploration.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <unistd.h>
#include <sys/wait.h>

/////////////////////
// HELPER FUNCTION //
/////////////////////

#ifndef __cplusplus11
inline std::string int_to_string(int i) {
    std::stringstream s;
    s << i;
    return s.str();
}
#endif

extern kc::tFormula TheFormula;


StateEquationTask::StateEquationTask()
{
	taskname = "state equation";
}

StateEquationTask::~StateEquationTask()
{
}

ternary_t StateEquationTask::getResult()
{

    // reset 
    saraIsRunning = 0; // not yet started
    // extract state predicate from formula
    assert(TheFormula);
    // copy formula for additional dnf rewrite
    kc::tFormula TheFormulaDNF;
    TheFormulaDNF = reinterpret_cast<kc::tFormula> (TheFormula->copy(true));
    assert(TheFormulaDNF);

    TheFormulaDNF = TheFormulaDNF->rewrite(kc::singletemporal);
    TheFormulaDNF = TheFormulaDNF->rewrite(kc::simpleneg);

    // get the assumed length of DNF formula and the assumed number of ORs
    unparsed.clear();
    TheFormulaDNF->unparse(myprinter, kc::internal);
    StatePredicate * TheStatePredicate = TheFormulaDNF->formula;
	//RT::rep->status("formula: %s",TheStatePredicate->toString());

    // unparse the content of the problem file for sara
	AtomicBooleanPredicate * dnf = TheStatePredicate->DNF();

	// check pathological cases in dnf
	if(dnf)
	{
		RT::data["task"]["stateequation"]["literals"] = static_cast<int>(dnf->literals);
		RT::data["task"]["stateequation"]["problems"] = static_cast<int>(dnf->cardSub);
	}
	else
	{
		RT::data["task"]["stateequation"]["literals"] = JSON::null;
		RT::data["task"]["stateequation"]["problems"] = JSON::null;
	}

	if(!dnf)
	{
		RT::rep->status("state equation: Could not create input for sara: DNF too long or DEADLOCK predicate contained");
		return TERNARY_UNKNOWN;
	}
	if(dnf -> magicnumber == MAGIC_NUMBER_TRUE)
	{
		RT::rep->status("state equation: State predicate is tautology.");
		return TERNARY_TRUE;
	}
	if(dnf -> magicnumber == MAGIC_NUMBER_FALSE)
	{
		RT::rep->status("state equation: State predicate is contradiction.");
		return TERNARY_FALSE;
	}
	if((!(dnf -> isAnd)) && (dnf->cardSub == 0))
	{
		RT::rep->status("state equation: State predicate is contradiction.");
		return TERNARY_FALSE;
	}
	if((dnf -> isAnd) && (dnf->cardSub == 0))
	{
		RT::rep->status("state equation: State predicate is tautology.");
		return TERNARY_TRUE;
	}

	if(dnf->isAnd)
	{
		AtomicBooleanPredicate * temp = new AtomicBooleanPredicate(false);
		temp -> addSub(dnf);
		dnf = temp;
	}

	//RT::rep->status("dnf: %s",dnf->toString());
	RT::rep->status("state equation: Generated DNF with %llu literals and  %llu conjunctive subformulas",dnf->literals,dnf->cardSub);
	//RT::rep->status("%s",dnf->toString());

	// write problem file for sara

	// preprare files
    static std::string baseFileName = "";
    static std::string saraProblemFileName = "";
    static std::string saraResultFileName = "";

    std::string formulaNumber = "";
    // check if formula is a compound formula
    if (RT::compoundNumber > 1)
    {
        // compound formula, add formula counter to the file name
#ifdef __cplusplus11
        formulaNumber = "-" + std::to_string(RT::compoundNumber);
#else
        formulaNumber = "-" + int_to_string(RT::compoundNumber);
#endif
    }
    
    // check if the task was given by a file
    if (RT::inputFormulaFileName != "")
    {
        // formula file is given
        baseFileName = RT::inputFormulaFileName;
        // remove file extension if present
        const size_t period_idx = baseFileName.rfind('.');
        if (std::string::npos != period_idx)
        {
            baseFileName.erase(period_idx);
        }
        saraProblemFileName = baseFileName + formulaNumber + ".sara";
        saraResultFileName = baseFileName + formulaNumber + ".sararesult";
    }
    else
    {
        // No formula file is given
        saraProblemFileName = "stateEquationProblem" + formulaNumber + ".sara";
        saraResultFileName = "stateEquationProblem" + formulaNumber + ".sararesult";
    }

    // delete old problem and result file
    std::remove(saraProblemFileName.c_str());
    std::remove(saraResultFileName.c_str());
    // Create new problem file
    std::ofstream lolafile(saraProblemFileName.c_str(), std::ios::out);

    // check if file could be created
    if (!lolafile)
    {
        // Problem file could not be constructed
        RT::rep->status("state equation: input file construction for sara not successful");
    }
    else
    {
        // Problem file was constructed
        RT::rep->status("state equation: write sara problem file to %s", 
                RT::rep->markup(MARKUP_FILE, saraProblemFileName.c_str()).str());
        // Write problem in file
	
	for(arrayindex_t i = 0; i < dnf->cardSub; i++)
	{
		// start single problem
		
		lolafile << "PROBLEM saraProblem:" << std::endl;
		lolafile << "GOAL REACHABILITY;" << std::endl;
		lolafile << "FILE " << RT::args.inputs[0] << " TYPE LOLA;" << std::endl;
		lolafile << "INITIAL ";
		bool needscomma = false;
		for(arrayindex_t j = 0; j < Net::Card[PL]; j++)
		{
			if(Marking::Initial[j] == 0) continue;
			if(needscomma) lolafile << ",";
			lolafile << Net::Name[PL][j] << ":" << Marking::Initial[j];
			needscomma = true;
		}
		lolafile << ";" << std::endl;
		lolafile << "FINAL COVER;" << std::endl;
		lolafile << "CONSTRAINTS ";

		AtomicBooleanPredicate * conj = (AtomicBooleanPredicate *) (dnf->sub[i]);
		
		needscomma = false;
		for(arrayindex_t j = 0; j < conj -> cardSub;j++)
		{
			AtomicStatePredicate * atomic = (AtomicStatePredicate *) (conj->sub[j]);

			if(needscomma) lolafile << ",";
			needscomma = true;
			bool needsplus = false;
			for(arrayindex_t k = 0; k < atomic->cardPos;k++)
			{
				if(needsplus) lolafile << " + ";
				if(atomic->posMult[k] != 1) lolafile << atomic->posMult[k];
				lolafile << Net::Name[PL][atomic->posPlaces[k]];
				needsplus = true;
			}
			for(arrayindex_t k = 0; k < atomic->cardNeg;k++)
			{
				if(needsplus) lolafile << " + ";
				lolafile << "-" << atomic->negMult[k];
				lolafile << Net::Name[PL][atomic->negPlaces[k]];
				needsplus = true;
			}
			lolafile << " < " << atomic->threshold;
		}
		lolafile << ";" << std::endl;
	}
        lolafile.close();
    }
    
    // result is as default unknown
    ternary_t result(TERNARY_UNKNOWN);
    
    // fork to get a PID for sara to kill her, if necessary
    pid_t pid_sara = fork();
    if (pid_sara < 0)
    {
        // fork failed
        RT::rep->status("state equation: cannot launch process for handling problem with sara");
        return TERNARY_UNKNOWN;
    }
    else if (pid_sara == 0)
    {
        // fork worked - child process
        // call sara
        RT::rep->status("state equation: calling and running sara");
        // set flag, that sara is running for the status message
        saraIsRunning = 1;
        execlp("sara", "sara", "-i", saraProblemFileName.c_str(), "-v", "-o",
                saraResultFileName.c_str(), (char*) 0);
        // call sara failed
        saraIsRunning = 2;
        RT::rep->status("state equation: call sara failed (perhaps sara is not installed)");
        // exit this process, otherwise we would have two processes
        exit(1);
    }
    else
    {
        saraIsRunning = 1;
        // parent process after fork succeeded
        // set saraPID to kill sara if necessary
        RT::saraPID = pid_sara;
        // save sara's process status
        int status = 0;
        // wait until sara is finished
        waitpid(pid_sara, &status, 0);
        if (WIFEXITED(status) == true)
        {
            // sara finished - check sara's result
            if (system(("grep -q 'SOLUTION' " + saraResultFileName).c_str()) == 0)
            {
                // solution found - result is true
                result = TERNARY_TRUE;
                RT::rep->status("state equation: solution produced");
            }
            else if (system(("grep -q 'INFEASIBLE' " + saraResultFileName).c_str()) == 0)
            {
                // solution is infeasible - result is false
                result = TERNARY_FALSE;
                RT::rep->status("state equation: solution impossible");
            }
            else
            {
                // sara can't decide the problem - result is unknonw
                result = TERNARY_UNKNOWN;
                RT::rep->status("state equation:solution unknown");
            }
        }
    }
    return result;
}

void StateEquationTask::interpreteResult(ternary_t result)
{
    switch (result)
    {
        case TERNARY_TRUE:
            RT::rep->status("result: %s", RT::rep->markup(MARKUP_GOOD, "yes").str());
            RT::rep->status("produced by: %s", taskname);
            RT::data["result"]["value"] = true;
            RT::data["result"]["produced_by"] = std::string(taskname);
            RT::rep->status("%s", RT::rep->markup(MARKUP_GOOD, "The predicate is reachable.").str());
            break;
        case TERNARY_FALSE:
            RT::rep->status("result: %s", RT::rep->markup(MARKUP_BAD, "no").str());
            RT::rep->status("produced by: %s", taskname);
            RT::data["result"]["value"] = false;
            RT::data["result"]["produced_by"] = std::string(taskname);
            RT::rep->status("%s", RT::rep->markup(MARKUP_BAD, "The predicate is unreachable.").str());
            break;

        case TERNARY_UNKNOWN:
            RT::rep->status("result: %s", RT::rep->markup(MARKUP_WARNING, "unknown").str());
            RT::rep->status("produced by: %s", taskname);
            RT::data["result"]["value"] = JSON::null;
            RT::data["result"]["produced_by"] = std::string(taskname);
            RT::rep->status("%s", RT::rep->markup(MARKUP_WARNING,
                    "The predicate may or may not be reachable.").str());
            break;
    }
}

Path StateEquationTask::getWitnessPath()
{
    // \todo add witness path
    Path * p = new Path();

    return *p;
}

capacity_t * StateEquationTask::getMarking()
{
    return NULL;
}

void StateEquationTask::getStatistics()
{
}

char * StateEquationTask::getStatus(uint64_t elapsed)
{
    char * result;
    result = new char[STATUSLENGTH];
    switch (saraIsRunning)
    {
    case 0: sprintf(result, "sara not yet started (preprocessing)");
	    break;
    case 1: sprintf(result, "sara is running %5llu secs", elapsed);
	    break;
    case 2: sprintf(result, "could not launch sara");
	    break;
    default:sprintf(result, "unknwon sara status");
    }
    return result;
}

char * StateEquationTask::early_abortion()
{
    return NULL;
}

Task * StateEquationTask::buildTask()
{
    return new StateEquationTask();
}


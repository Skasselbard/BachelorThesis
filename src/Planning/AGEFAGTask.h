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

#include <Core/Dimensions.h>
#include <Witness/Path.h>
#include <Planning/Task.h>
#include <Planning/EFAGEFTask.h>

/*!
\brief the verification task

This class delegates the AGEFAG problem to the EFAGEF problem.
It adjusts the return value and the result interpretation.

*/

class AGEFAGTask : public Task
{
public:
     Task * D;   // the corresponding EFAGEFTask we are delegating to
    ~AGEFAGTask(){}

    /// run the actual verification algorithm
    ternary_t getResult() {
       taskname = D->taskname;
	ternary_t result = D -> getResult();  // run the EFAGEF task
	switch(result)   // reverse return value
	{
	case TERNARY_TRUE: return TERNARY_FALSE;
	case TERNARY_FALSE: return TERNARY_TRUE;
	case TERNARY_UNKNOWN: return TERNARY_UNKNOWN;
	default: assert(false);
	}
	return result;
   }

    /// interprete and display the result
    void interpreteResult(ternary_t result) {
	switch(result)
	{
case TERNARY_TRUE:
        RT::rep->status("result: %s", RT::rep->markup(MARKUP_GOOD, "yes").str());
        RT::rep->status("produced by: %s", taskname);
        RT::data["result"]["value"] = true;
        RT::data["result"]["produced_by"] = std::string(taskname);

            RT::rep->status("%s", RT::rep->markup(MARKUP_GOOD, "The predicate is always possibly invariant.").str());

        break;

 case TERNARY_FALSE:
        RT::rep->status("result: %s", RT::rep->markup(MARKUP_BAD, "no").str());
        RT::rep->status("produced by: %s", taskname);
        RT::data["result"]["value"] = false;
        RT::data["result"]["produced_by"] = std::string(taskname);

            RT::rep->status("%s", RT::rep->markup(MARKUP_BAD,
                                                  "The predicate is not always possibly invariant.").str());
        break;
  case TERNARY_UNKNOWN:
        RT::rep->status("result: %s", RT::rep->markup(MARKUP_WARNING, "unknown").str());
        RT::rep->status("produced by: %s", taskname);
        RT::data["result"]["value"] = JSON::null;
        RT::data["result"]["produced_by"] = std::string(taskname);

            RT::rep->status("%s", RT::rep->markup(MARKUP_WARNING,
                                                  "The predicate may or may not be always possibly invariant.").str());
        break;
    }

   }

    /// return the witness path
    Path getWitnessPath() { return D -> getWitnessPath();}
    /// return the target marking
    capacity_t *getMarking() { return D -> getMarking();}

    /// return the number of stored markings
    void getStatistics() {D -> getStatistics();}
    static Task * buildTask() {
	AGEFAGTask * N = new AGEFAGTask();
	N -> D = EFAGEFTask::buildTask();
	return N;
    }
    /// fetch a status message
    char * getStatus(uint64_t elapsed){return D -> getStatus(elapsed);} 
    /// check whether we should abort
    char * early_abortion(){return D -> early_abortion(); }
    StatePredicate * spFormula;
};

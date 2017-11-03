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

#include <Core/Dimensions.h>
#include <Core/Runtime.h>
#include <InputOutput/InputOutput.h>
#include <Formula/LTL/BuechiAutomata.h>
#include <Formula/StatePredicate/AtomicStatePredicate.h>
#include <Net/Net.h>
#include <Net/NetState.h>


bool BuechiAutomata::isAcceptingState(arrayindex_t state)
{
    return isStateAccepting[state];
}

arrayindex_t BuechiAutomata::getNumberOfStates()
{
    return cardStates;
}

BuechiAutomata::~BuechiAutomata()
{
    for (arrayindex_t i = 0; i < cardStates; i++)
    {
        delete[] nextstate[i];
        delete[] guard[i];
    }

    delete[] nextstate;
    delete[] guard;
    delete[] cardTransitions;
    delete[] isStateAccepting;
}


int current_next_string_index_number = 1;

char *produce_next_string(int *val)
{
    current_next_string_index_number++;
    int length = (int) log10(current_next_string_index_number) + 2;
    char *buf = new char[length]();
    sprintf(buf, "%d", current_next_string_index_number);
    *val = current_next_string_index_number;
    return buf;
}

void BuechiAutomata::writeBuechi()
{
    Output buechifile("Buechi automaton", std::string(RT::args.writeBuechi_arg) + ".buechi");
    fprintf(buechifile, "buechi\n{\n");
    for (int i = 0; i < cardStates; i++)
    {
        fprintf(buechifile, "\tState%d :\n", i);
        for (int j = 0; j < cardTransitions[i]; j++)
        {
            fprintf(buechifile, "\t\t%s => State%d\n", guard[i][j]->toString(), nextstate[i][j]);
        }
        if (i < cardStates - 1)
        {
            fprintf(buechifile, "\t\t,\n");
        }
    }
    fprintf(buechifile, "}\naccept\n{\n");
    bool notfirst = false;
    for (int k = 0; k < cardStates; k++)
    {
        if (isStateAccepting[k])
        {
            if (notfirst)
            {
                fprintf(buechifile, " ,\n");
            }
            notfirst = true;
            fprintf(buechifile, "\tState%d", k);
        }
    }
    fprintf(buechifile, "\n};\n");
}

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
\author Niels
\status new

\brief Definition of a struct collecting runtime information.
*/

#pragma once

#include <config.h>
#include <cmdline.h>
#include <InputOutput/InputOutput.h>
#include <InputOutput/JSON.h>
#include <InputOutput/Reporter.h>
#include <pthread.h>

struct RT
{
public:
    static JSON data;
    static Reporter *rep;

    static gengetopt_args_info args;

    static void initialize(int argc, char **argv);

    static Input *currentInputFile;

    /// a thread that runs the reporter_thread function
    static pthread_t reporter_thread;

private:
    /// the reporter function
    static void *reporter_internal(void*);

    static void callHome();
    static void evaluateParameters(int argc, char **argv);
};

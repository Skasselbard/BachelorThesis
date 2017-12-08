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
#ifndef BENCHMARK
#define BENCHMARK
/*!
\file
\author Tom Meyer
\status new

\brief Class to handle benchmarks to analyze LoLA
*/
#include <list>
#include <string>
#include <chrono>
#include <ratio>

#include "Core/Dimensions.h"

using namespace std::chrono;

class Benchmark{
public:
    static Benchmark* newBenchmark(std::string name, int numberOfThreads);
    ~Benchmark();
    void startCycle(threadid_t threadID);
    void endCycle(threadid_t threadID);

    /// Writes the meassurements to a fie
    /// DELETES ALL MESSURED DATA
    static void writeToFile(std::string fileName);

private:
    explicit Benchmark(){}
    static std::list<Benchmark*> allBenchmarks;

    duration<double, std::nano>* benchmarkArray;
    int arraySize;

    int benchmarkInt;
    std::string benchmarkName;

    high_resolution_clock::time_point lastStart;

};

#endif
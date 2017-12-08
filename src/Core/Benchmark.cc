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
\author Tom Meyer
\status new

*/
#include <iostream>
#include <fstream>
#include <sstream>

#include "Benchmark.h"
#include "Runtime.h"

std::list<Benchmark*> Benchmark::allBenchmarks = std::list<Benchmark*>();

Benchmark* Benchmark::newBenchmark(std::string name, int numberOfThreads){
    Benchmark* current = new Benchmark();
    current->benchmarkName = name;
    
    current->benchmarkArray = new duration<double, std::nano>[numberOfThreads]();
    current->arraySize = numberOfThreads;
    for(int i = 0; i < numberOfThreads; i++){
        current->benchmarkArray[i] = duration<double, std::nano>(0);
    }
    Benchmark::allBenchmarks.push_back(current);
    return current;
}

Benchmark::~Benchmark(){
    if (benchmarkArray){
        delete[] benchmarkArray;
    }
    Benchmark::allBenchmarks.remove(this);
}

void Benchmark::startCycle(threadid_t threadID){
    if (benchmarkArray){
        lastStart= high_resolution_clock::now();
    }
}
void Benchmark::endCycle(threadid_t threadID){
    if (benchmarkArray){
        benchmarkArray[threadID] += 
            duration_cast<nanoseconds>((high_resolution_clock::now())-lastStart);
    }
}

void Benchmark::writeToFile(std::string fileName){
    std::ofstream dfsFile;
    dfsFile.open("dfsTime.txt");
    if (dfsFile){
        while (!Benchmark::allBenchmarks.empty()){
            Benchmark* currentBenchmark = Benchmark::allBenchmarks.front();
            // dfsFile << "Cumulative DFS time: ";
            // dfsFile << dfsTime.count();
            // dfsFile << "ns\n";
            // dfsFile << "Cumulative hand over time: ";
            // dfsFile << calcCumulativeTime(timeToHandOverWork, _number_of_threads).count();
            // dfsFile << "ns\n";
            // dfsFile << "Cumulative idle time: ";
            // dfsFile << calcCumulativeTime(threadIdleTimes, _number_of_threads).count();
            // dfsFile << "ns\n";
            // dfsFile << "Cumulative firelist fetching time: ";
            // dfsFile << calcCumulativeTime(firelistFetchTime, _number_of_threads).count();
            // dfsFile << "ns\n";
            // dfsFile << "Cumulative synchronization time: ";
            // dfsFile << calcCumulativeTime(threadSyncTimes, _number_of_threads).count();
            // dfsFile << "ns\n\n";
            for(int i = 0; i < currentBenchmark->arraySize; i++){
                //dfsFile << "Thread " << i << "\n";
                dfsFile << std::defaultfloat << currentBenchmark->benchmarkArray[i].count()/1000000000;
                dfsFile << "s\t";
                dfsFile << currentBenchmark->benchmarkName;
                dfsFile << "\n";
            }
            dfsFile << "\n";
            Benchmark::allBenchmarks.pop_front();
            delete currentBenchmark;
        }
        dfsFile.close();
    }else{
        RT::rep->status("unable to write dfs log");
    }
}

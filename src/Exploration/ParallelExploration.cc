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
\author Gregor
\status new

\brief Evaluates simple property (only SET of states needs to be computed) in
parallel.
*/

#include <Core/Dimensions.h>
#include <Core/Runtime.h>
#include <CoverGraph/CoverGraph.h>
#include <Exploration/DFSExploration.h>
#include <Exploration/ParallelExploration.h>
#include <Net/Net.h>
#include <SweepLine/Sweep.h>
#include <iostream>
#include <fstream>

/// transfer struct for the start of a parallel search thread
struct tpDFSArguments
{
    /// the store to use (this will be the same of all threads)
    /// the number of the current thread
    int threadNumber;
    /// the exploration object
    ParallelExploration *pexploration;
};

ParallelExploration::ParallelExploration():    
// initialize mutexes
global_property_mutex((bool)UNLOCKED),
num_suspend_mutex((bool)UNLOCKED),
threadIdleTimes(nullptr),
timeToHandOverWork(nullptr),
threadSyncTimes(nullptr)
{}

void *ParallelExploration::threadPrivateDFS(void *container)
{
    tpDFSArguments *arguments = static_cast<tpDFSArguments *>(container);

    return arguments->pexploration->threadedExploration(arguments->threadNumber);
}

NetState *ParallelExploration::threadedExploration(threadid_t threadNumber){
    SimpleProperty *local_property = thread_property[threadNumber];
    Firelist *local_firelist = global_baseFireList->createNewFireList(local_property);
    SearchStack<SimpleStackEntry> local_stack = thread_stack[threadNumber];
    NetState local_netstate = thread_netstate[threadNumber];

    // prepare property
    bool propertyResult = local_property->initProperty(local_netstate);

    if (propertyResult){
        // initial marking satisfies property
        // inform all threads that a satisfying has been found
        finished = true;
        releaseRestartSemaphores();
        publishLocalState(local_stack, local_netstate, threadNumber);
        delete local_firelist;
        return &local_netstate;
    }

    // add initial marking to store
    // we do not care about return value since we know that store is empty
    global_store->searchAndInsert(local_netstate, 0, threadNumber);
    // get first firelist
    arrayindex_t* currentFirelist;
    arrayindex_t currentEntry = local_firelist->getFirelist(local_netstate, &currentFirelist);

    while (true){ // exit when trying to pop from empty stack
        // if one of the threads sets the finished signal this thread is useless and has to abort
        if (finished)
        {
            // clean up and return
            delete[] currentFirelist;
            delete local_firelist;
            return NULL;
        }
        if (currentEntry-- > 0)//check if there are entries left and decrement the index
        {
            // there is a next transition that needs to be explored in current marking
            // fire this transition to produce new marking in ns
            Transition::fire(local_netstate, currentFirelist[currentEntry]);
            if (global_store->searchAndInsert(local_netstate, 0, threadNumber))
            {
                // State exists! --> backtrack to previous state and try again
                Transition::backfire(local_netstate, currentFirelist[currentEntry]);
                continue;
            }
            // State does not exist!

            // check whether the new state fulfills the property
            Transition::updateEnabled(local_netstate, currentFirelist[currentEntry]);
            // check current marking for property
            propertyResult = local_property->checkProperty(local_netstate, currentFirelist[currentEntry]);

            if (propertyResult){
                // inform all threads that a satisfying state has been found
                finished = true;
                releaseRestartSemaphores();
                publishLocalState(local_stack, local_netstate, threadNumber);
                delete[] currentFirelist;
                delete local_firelist;
                return &local_netstate;
            }
            safeState(local_stack, currentFirelist, currentEntry);

            // if possible spare the current transition to give it to an other thread, having explored it's part of the search space completely
            // try the dirty read to make the program more efficient
            // most of the time this will already fail and we have saved the time needed to lock the mutex
            // ... do it only if there are at least two transitions in the firelist left (one for us, and one for the other thread)
            auto startOfHandoverCheck = high_resolution_clock::now();
            if (canHandOverWork(currentEntry)){
                auto startSync = high_resolution_clock::now();
                waitAndLock(&num_suspend_mutex);
                threadSyncTimes[threadNumber] += duration_cast<microseconds>((high_resolution_clock::now())-startSync);
                if (num_suspended > 0){
                    HandOverWork(local_property, local_stack, local_netstate);
                    if (finished){
                        // clean up and return
                        delete local_firelist;
                        return NULL;
                    }
                    restoreState(local_stack, local_netstate, currentEntry, currentFirelist);
                    local_property->updateProperty(local_netstate, currentFirelist[currentEntry]);
                    // go on as nothing happened (i.e. pretend the new marking has already been in the store)
                    continue;
                }
                unlock(&num_suspend_mutex);
            }
            timeToHandOverWork[threadNumber] += duration_cast<microseconds>((high_resolution_clock::now())-startOfHandoverCheck);
            // current marking does not satisfy property --> continue search
            // grab a new firelist (old one is already on stack)
            currentEntry = local_firelist->getFirelist(local_netstate, &currentFirelist);
        }
        else
        {
            // firing list completed -->close state and return to previous state
            delete[] currentFirelist;

            if (local_stack.StackPointer){
                // if there is still a firelist, which can be popped, do it!
                backtrack(local_stack, local_netstate, currentEntry, currentFirelist);
                propertyResult = local_property->updateProperty(local_netstate, currentFirelist[currentEntry]);
                continue;
            }

            // we have completely processed local initial marking --> state not found in current sub-tree

            // delete firelist (we will get new ones if we start again in a different sub-tree)
            delete local_firelist;

            // maybe we have to go into an other sub-tree of the state-space
            // first get the counter mutex to be able to count the number of threads currently suspended
            int local_num_suspended = declareJoblessness(threadNumber);

            // if we are the last thread going asleep, we have to wake up all the others and tell them that the search is over
            if (local_num_suspended == number_of_threads){
                finished = true;
                releaseRestartSemaphores();
                return NULL; // there is no such state
            }
            auto startOfIdle = high_resolution_clock::now();
            lock(restartSemaphore[threadNumber]);
            waitForUnlock(restartSemaphore[threadNumber]);
            threadIdleTimes[threadNumber] += duration_cast<microseconds>((high_resolution_clock::now())-startOfIdle);

            // test if result is already known now
            if (finished)            {
                return NULL;
            }

            // rebuild
            local_property->initProperty(local_netstate);
            local_firelist = global_baseFireList->createNewFireList(local_property);

            // re-initialize the current search state
            currentEntry = local_firelist->getFirelist(local_netstate, &currentFirelist);
        }
    }
}

bool ParallelExploration::depth_first(SimpleProperty &property, NetState &ns,
                                      Store<void> &myStore, Firelist &firelist,
                                      int _number_of_threads)
{
    // initialize benchmark arrays
    auto startOfDFS = high_resolution_clock::now();
    threadIdleTimes = new microseconds[_number_of_threads]();
    timeToHandOverWork = new microseconds[_number_of_threads]();
    threadSyncTimes = new microseconds[_number_of_threads]();
    for(int i = 0; i < _number_of_threads; i++){
        threadIdleTimes[i] = microseconds(0);
        timeToHandOverWork[i] = microseconds(0);
        threadSyncTimes[i] = microseconds(0);
    }
    // allocate space for threads
    threads = new pthread_t[_number_of_threads]();

    global_property = &property;
    global_store = &myStore;
    global_baseFireList = &firelist;
    number_of_threads = static_cast<threadid_t>(_number_of_threads);

    // allocate space for thread arguments
    tpDFSArguments *args = new tpDFSArguments[number_of_threads]();
    for (int i = 0; i < number_of_threads; i++)
    {
        args[i].threadNumber = i;
        args[i].pexploration = this;
    }

    // allocate and initialize thread local information
    thread_netstate = new NetState[number_of_threads];
    thread_property = new SimpleProperty*[number_of_threads];
    thread_stack = new SearchStack<SimpleStackEntry>[number_of_threads];
    for (int i = 0; i < number_of_threads; i++){
        NetState tmp_netstate(ns);
        thread_netstate[i].swap(tmp_netstate); // copy and swap
        thread_property[i] = property.copy();
    }

    // init the restart semaphore
    restartSemaphore = new std::atomic<bool>*[number_of_threads];
    for (int i = 0; i < number_of_threads; i++){
        std::atomic<bool>* currentSemaphore = new std::atomic<bool>((bool)LOCKED);
        restartSemaphore[i] = currentSemaphore;
    }

    // initialize thread intercommunication data structures
    num_suspended = 0;
    suspended_threads = new arrayindex_t[number_of_threads];
    finished = false;

    // start the threads
    for (int i = 0; i < number_of_threads; i++)
        if (UNLIKELY(pthread_create(threads + i, NULL, threadPrivateDFS, args + i)))
        {
            // LCOV_EXCL_START
            RT::rep->status("threads could not be created");
            RT::rep->abort(ERROR_THREADING);
            // LCOV_EXCL_STOP
        }

    //// THREADS ARE RUNNING AND SEARCHING

    // wait for all threads to finish
    property.value = false;
    for (int i = 0; i < number_of_threads; i++)
    {
        void *return_value;
        if (UNLIKELY(pthread_join(threads[i], &return_value)))
        {
            // LCOV_EXCL_START
            RT::rep->status("threads could not be joined");
            RT::rep->abort(ERROR_THREADING);
            // LCOV_EXCL_STOP
        }
        if (return_value)
        {
            property.value = true;
            ns = *reinterpret_cast<NetState *>(return_value);
        }
    }

    // clean up semaphores
    for (int i = 0; i < number_of_threads; i++){
        delete restartSemaphore[i];
    }

    // clean up thread intercommunication data structures
    for (int i = 0; i < number_of_threads; i++)
    {
        delete thread_property[i];
    }
    delete[] suspended_threads;
    delete[] thread_netstate;
    delete[] thread_property;
    delete[] thread_stack;

    // clean up thread arguments
    delete[] args;

    // free the allocated memory
    delete[] restartSemaphore;
    delete[] threads;

    // write benchmark
    std::ofstream dfsFile;
    dfsFile.open ("dfsTime.txt");
    if (dfsFile){
        dfsFile << "Cumulative DFS time: " 
        << duration_cast<workTime>(high_resolution_clock::now() - startOfDFS).count() 
        << "µs\n";
        dfsFile << "Cumulative hand over time: " 
        << calcCumulativeTime(timeToHandOverWork, _number_of_threads).count() 
        << "µs\n";
        dfsFile << "Cumulative idle time: " 
        << calcCumulativeTime(threadIdleTimes, _number_of_threads).count()  
        << "µs\n";
        dfsFile << "Cumulative synchronization time: " 
        << calcCumulativeTime(threadSyncTimes, _number_of_threads).count() 
        << "µs\n\n";
        for(int i = 0; i < _number_of_threads; i++){
            dfsFile << "[Thread " << i << "]\n";
            dfsFile << "Time for hand over checks: ";
            dfsFile << timeToHandOverWork[i].count();
            dfsFile << "µs\n";
            dfsFile << "Idle time: ";
            dfsFile << threadIdleTimes[i].count();
            dfsFile << "µs\n";
            dfsFile << "Synchronization time: ";
            dfsFile << threadSyncTimes[i].count();
            dfsFile << "µs\n";
            dfsFile << "\n";
        }
        dfsFile.close();
    }

    delete[] threadIdleTimes;
    delete[] timeToHandOverWork;
    delete[] threadSyncTimes;
    return property.value;
}

bool ParallelExploration::sweepline(SimpleProperty &property, NetState &ns,
                                    SweepEmptyStore &myStore, Firelist &myFirelist, int frontNumber, threadid_t threadNumber)
{
    Sweep<void> s(property, ns, myStore, myFirelist, frontNumber, threadNumber);
    return s.runThreads();
}

ternary_t ParallelExploration::cover_breadth_first(SimpleProperty &property, NetState &ns,
        Store<CoverPayload> &myStore,
        Firelist &myFirelist, threadid_t threadNumber, formula_t type)
{
    CoverGraph c(property, ns, myStore, myFirelist, _p, threadNumber);
    if (type == FORMULA_LIVENESS)
    {
        c.setAGEF();
    }
    return c.runThreads();
}

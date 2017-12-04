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
#ifndef LOLA_MUTEX
#define LOLA_MUTEX
/*!
\file LolaMutex
\author Tom Meyer
\status new

\brief Functions to use a mutex like structure for lola

These functions do not provide any safety features like e.g mutexes from the pthread
library. Especially no guards against deadlocks are implemented.
*/

#include<atomic>
 

class LolaMutex{
private:
    /// The internal state of the mutex
    std::atomic<bool> locked;

    enum LOCK{
        LOCKED,
        UNLOCKED
    };
public:
    LolaMutex(bool unlocked=true):locked(unlocked){
    }

    /// Blocks until the internal state is UNLOCKED and then emediatly sets the state to LOCKED
    /// Will wait for eternity if the state won't be unlocked by another thread
    void waitForLock(){
        bool expected = UNLOCKED;
        while (!locked.compare_exchange_strong(expected,LOCKED)){
            expected = UNLOCKED;
        }
    }
    /// Blocks until the internal state is UNLOCKED
    /// Does not lock the state again
    void waitForUnlock(){
        while (locked.load() != UNLOCKED){
            continue;
        }
    }
    /// Sets the internal state to LOCKED
    /// Does NOT CARE if the internal state is UNLOCKED or LOCKED and always returns
    void lock(){
        locked.store(LOCKED);
    }
    /// Sets the internal state to UNLOCKED
    /// Does NOT CARE if the internal state is UNLOCKED or LOCKED and always returns
    void unlock(){
        locked.store(UNLOCKED);
    }
};

#endif //LOLA_MUTEX
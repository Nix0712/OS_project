#pragma once

#include "../h/List.hpp"
#include "../h/TCB.hpp"

class Scheduler {

  private:
    static uint64 time;
    static Queue readyThreadQueue;    // Queue of ready threads
    static Queue sleepingThreadQueue; // Queue of threads that were put to sleep by time_sleep (it's sorted)
    static Queue timedThreadQueue;    // Queue of threads that were put to sleep by semaphore timedwait (it's sorted)

  public:
    static void putReady(TCB* tcb);
    static void putSleeping(TCB* tcb, time_t wakeUpTime);
    static void putTimed(TCB* tcb, time_t wakeUpTime);
    static void removeTimed(TCB* tcb);
    static TCB* getReady();
    static uint64 getTime();
    static void updateTime();
    static void updateSleeping();
    static void updateTimed();
};

#pragma once

#include "../h/List.hpp"
#include "../h/TCB.hpp"

class Scheduler {

  private:
    static uint64 time;
    static Queue readyThreadQueue;
    static Queue sleepingThreadQueue;
    static Queue terminatedThreadQueue;

  public:
    static void putReady(TCB* tcb);
    static void putSleeping(TCB* tcb, time_t wakeUpTime);
    static void putTerminated(TCB* tcb, time_t wakeUpTime);
    static TCB* getReady();
    static void updateTime();
    static void updateSleeping();
    static void updateTerminated();
};

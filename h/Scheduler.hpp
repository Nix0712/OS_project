#pragma once

#include "../h/List.h"
#include "../h/TCB.hpp"

class Scheduler {

    class SleepingNode {
      public:
        TCB* tcb;
        time_t wakeUpTime;
    };

  private:
    static uint64 time;
    static LinkedList<TCB> readyThreadQueue;
    static LinkedList<SleepingNode> sleepingThreadQueue;
    static LinkedList<SleepingNode> waitingThreadQueue;

  public:
    static TCB* getReady();

    static void putReady(TCB* tcb);

    static void putSleeping(TCB* tcb, time_t wakeUpTime);

    static void updateSleeping();

    static void putWaiting(TCB* tcb);

    static void updateWaiting();

    static uint64 getTime();
};

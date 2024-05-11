#pragma once

#include "../h/List.h"
#include "../h/TCB.hpp"

class Scheduler {

  private:
    static LinkedList<TCB> readyThreadQueue;

  public:
    static TCB* getCoroutine();

    static void putCoroutine(TCB* tcb);
};

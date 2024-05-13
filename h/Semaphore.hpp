#pragma once

#include "../h/List.hpp"
#include "../h/Scheduler.hpp"
#include "../h/TCB.hpp"
#include "hw.h"

class _Semaphore {
  public:
    _Semaphore(unsigned init = 1) : val(init), blocked() {}
    ~_Semaphore();

    void wait();
    void signal();
    void timedwait(time_t timeout);
    void trywait();

  protected:
    void block();
    void deblock();

  private:
    int val;
    Queue blocked;
};
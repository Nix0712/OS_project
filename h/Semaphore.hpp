#pragma once

#include "../h/List.hpp"
#include "../h/Scheduler.hpp"
#include "../h/TCB.hpp"
#include "hw.h"

class _Semaphore {
  public:
    _Semaphore(unsigned init = 1) : val(init), blocked() {}
    ~_Semaphore();

    int wait();
    void signal();
    int timedwait(time_t timeout);
    int trywait();

  protected:
    void block();
    void deblock();

  private:
    enum ErrorCodes {
        SEMDEAD = -1,
        TIMEOUT = -2,
    };
    int val;
    Queue blocked;
};
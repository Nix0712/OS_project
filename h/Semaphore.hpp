#pragma once

#include "hw.h"

class _Semaphore {
  public:
    _Semaphore(unsigned init);
    ~_Semaphore();

    void wait();
    void signal();
    void timedwait(time_t timeout);
    void trywait();

  private:
    unsigned val;
};
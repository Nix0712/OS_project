#pragma once

#include "../h/List.h"
#include "../h/Scheduler.hpp"
#include "../h/TCB.hpp"
#include "hw.h"

class _Semaphore {
  public:
    _Semaphore(unsigned init = 1) : val(init), timeout(0), TimedOut(false), wakeUpTime(0), isBlockedOnTry(false), blocked() {}
    ~_Semaphore();

    void wait();
    void signal();
    void timedwait(time_t timeout);
    void trywait();

    void setTimedOut(bool timedOut) { TimedOut = timedOut; }
    bool isTimedOut() { return TimedOut; }

    bool getIsBlockedOnTry() { return isBlockedOnTry; }

    time_t getTimeOut() { return timeout; }

  protected:
    void block();
    void deblock();

  private:
    int val;
    time_t timeout; // timeout for timedwait
    bool TimedOut;  // Wehn the semaphore is timed out after a timedwait
    time_t wakeUpTime;
    bool isBlockedOnTry;
    LinkedList<TCB> blocked;
};
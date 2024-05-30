#pragma once

#include "../h/List.hpp"
class _Semaphore {
  public:
    _Semaphore(unsigned init = 1) : val(init), blocked() {}
    ~_Semaphore();

    int wait();
    void signal();
    int timedwait(time_t timeout);
    int trywait();

    void removeFromBlockedList(TCB* tcb) {
        blocked.removeTCB(tcb);
    }

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
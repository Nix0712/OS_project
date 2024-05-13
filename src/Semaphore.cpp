#include "../h/Semaphore.hpp"
#include "../h/Scheduler.hpp"

_Semaphore::~_Semaphore() {
    TCB* curr = blocked.popTCB();
    while (curr) {
        Scheduler::putReady(curr);
        curr = blocked.popTCB();
    }
}

void _Semaphore::wait() {
    if (--val < 0)
        block();
}

void _Semaphore::signal() {
    if (val++ < 0)
        deblock();
}

void _Semaphore::timedwait(time_t timeout) {
}

void _Semaphore::trywait() {
}

void _Semaphore::block() {
    TCB::running->setReady(false);
    blocked.pushTCB(TCB::running);
    TCB::dispatch();
}

void _Semaphore::deblock() {
    TCB* tcb = blocked.popTCB();
    if (tcb) {
        Scheduler::putReady(tcb);
    }
}

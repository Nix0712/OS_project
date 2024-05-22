#include "../h/Semaphore.hpp"
#include "../h/Scheduler.hpp"

_Semaphore::~_Semaphore() {
    TCB* curr = blocked.popTCB();
    while (curr) {
        Scheduler::putReady(curr);

        curr = blocked.popTCB();
    }
}

int _Semaphore::wait() {
    if (--val < 0)
        block();

    if (TCB::running->isTimedWaitExpired())
        val++;
    // If semaphore was closed while waiting
    if (TCB::running->GetIsClosedInSemaphore()) {
        return -1;
    }
    return 0;
}

void _Semaphore::signal() {
    if (val++ < 0)
        deblock();
}

int _Semaphore::timedwait(time_t timeout) {
    TCB::running->setWaitTime(Scheduler::getTime() + timeout);
    TCB::running->setTimedWaitExpired(false);
    TCB::running->setSemaphore(this);
    Scheduler::putTimed(TCB::running, TCB::running->getWaitTime());
    int ret = wait();
    if (ret == -1)
        return SEMDEAD;
    else if (TCB::running->isTimedWaitExpired())
        return TIMEOUT;
    return 0;
}

int _Semaphore::trywait() {
    val--;
    if (val < 0) {
        return 0;
    } else
        return 1;
}

void _Semaphore::block() {
    TCB::running->setReady(false);
    blocked.pushTCB(TCB::running);
    TCB::dispatch();
}

void _Semaphore::deblock() {
    TCB* tcb = blocked.popTCB();
    if (!tcb->isTimedWaitExpired()) {
        Scheduler::removeTimed(tcb);
    }

    if (tcb) {
        Scheduler::putReady(tcb);
    }
}

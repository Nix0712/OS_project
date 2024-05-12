#include "../h/Semaphore.hpp"
#include "../h/Scheduler.hpp"

_Semaphore::~_Semaphore() {
    TCB* curr = (TCB*)blocked.removeFirst();
    while (curr) {
        if (curr->isFinished())
            curr->setWorkingSemaphore(-1);
        Scheduler::putReady(curr);
        curr = (TCB*)blocked.removeFirst();
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
    this->timeout = timeout;
    TimedOut = false;
    Scheduler::putWaiting(TCB::running);
    block();
}

void _Semaphore::trywait() {
    if (val - 1 < 0) {
        val--;
        isBlockedOnTry = true;
        block();
    } else
        isBlockedOnTry = false;
}

void _Semaphore::block() {
    TCB::running->setWorkingSemaphore(0);
    blocked.insertLast(TCB::running);
    TCB::dispatch();
}

void _Semaphore::deblock() {
    TCB* t = blocked.removeFirst();
    if (t)
        Scheduler::putReady(t);
}

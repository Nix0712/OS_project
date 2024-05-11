#include "../h/Scheduler.hpp"

LinkedList<TCB> Scheduler::readyThreadQueue;

void Scheduler::putCoroutine(TCB* coroutine) {
    readyThreadQueue.insertLast(coroutine);
}

TCB* Scheduler::getCoroutine() {

    TCB* ret = readyThreadQueue.removeFirst();
    return ret;
}
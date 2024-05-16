#include "../h/Scheduler.hpp"
#include "../h/MemoryAllocator.hpp"

uint64 Scheduler::time = 0;
Queue Scheduler::readyThreadQueue;
Queue Scheduler::sleepingThreadQueue;

void Scheduler::putReady(TCB* tcb) {
    readyThreadQueue.pushTCB(tcb);
    tcb->setReady(true);
}
void Scheduler::putSleeping(TCB* tcb, time_t wakeUpTime) {
    SleepingNode* sn = new SleepingNode;
    sn->tcb = tcb;
    sn->wakeUpTime = wakeUpTime + time;
    sleepingThreadQueue.pushSortedSTQ(sn);
    tcb->setReady(false);
}

TCB* Scheduler::getReady() {
    return readyThreadQueue.popTCB();
}

uint64 Scheduler::getTime() {
    return time;
}
void Scheduler::updateTime() {
    time++;
}

void Scheduler::updateSleeping() {
    while (sleepingThreadQueue.size() > 0) {
        SleepingNode* sn = sleepingThreadQueue.popSTQ();
        if (sn->wakeUpTime <= time) {
            putReady(sn->tcb);
            MemoryAllocator* memAllocator = MemoryAllocator::GetInstance();
            memAllocator->mem_free(sn);
        } else {
            sleepingThreadQueue.pushSortedSTQ(sn);
            break;
        }
    }
}

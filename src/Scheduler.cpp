#include "../h/Scheduler.hpp"
#include "../h/MemoryAllocator.hpp"

uint64 Scheduler::time = 0;
Queue Scheduler::readyThreadQueue;
Queue Scheduler::sleepingThreadQueue;
Queue Scheduler::terminatedThreadQueue;

void Scheduler::putReady(TCB* tcb) {
    readyThreadQueue.pushTCB(tcb);
    tcb->setReady(true);
}
void Scheduler::putSleeping(TCB* tcb, time_t wakeUpTime) {
    SleepingNode* sn = new SleepingNode;
    sn->tcb = tcb;
    sn->wakeUpTime = wakeUpTime;
    sleepingThreadQueue.pushSortedSTQ(sn);
    tcb->setReady(false);
}

void Scheduler::putTerminated(TCB* tcb, time_t wakeUpTime) {
    SleepingNode* sn = new SleepingNode;
    sn->tcb = tcb;
    sn->wakeUpTime = wakeUpTime;
    terminatedThreadQueue.pushSortedSTQ(sn);
    tcb->setReady(false);
}

TCB* Scheduler::getReady() {
    return readyThreadQueue.popTCB();
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

void Scheduler::updateTerminated() {
    while (terminatedThreadQueue.size() > 0) {
        SleepingNode* sn = terminatedThreadQueue.popSTQ();
        if (sn->wakeUpTime <= time) {
            putReady(sn->tcb);
            MemoryAllocator* memAllocator = MemoryAllocator::GetInstance();
            memAllocator->mem_free(sn);
        } else {
            terminatedThreadQueue.pushSortedSTQ(sn);
            break;
        }
    }
}

#include "../h/Scheduler.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/Semaphore.hpp"

uint64 Scheduler::time = 0;
Queue Scheduler::readyThreadQueue;
Queue Scheduler::sleepingThreadQueue;
Queue Scheduler::timedThreadQueue;

void Scheduler::putReady(TCB* tcb) {
    readyThreadQueue.pushTCB(tcb);
    tcb->setReady(true);
}
void Scheduler::putSleeping(TCB* tcb, time_t wakeUpTime) {
    size_t numOfBlocks = ((sizeof(SleepingNode) + MEM_BLOCK_SIZE - 1) + 24UL) / MEM_BLOCK_SIZE;
    SleepingNode* sn = (SleepingNode*)MemoryAllocator::GetInstance()->mem_alloc(numOfBlocks);
    sn->tcb = tcb;
    sn->wakeUpTime = wakeUpTime + time;
    sleepingThreadQueue.pushSortedSTQ(sn);
    tcb->setReady(false);
}

void Scheduler::putTimed(TCB* tcb, time_t wakeUpTime) {
    size_t numOfBlocks = ((sizeof(SleepingNode) + MEM_BLOCK_SIZE - 1) + 24UL) / MEM_BLOCK_SIZE;
    SleepingNode* sn = (SleepingNode*)MemoryAllocator::GetInstance()->mem_alloc(numOfBlocks);
    sn->tcb = tcb;
    sn->wakeUpTime = wakeUpTime;
    timedThreadQueue.pushSortedSTQ(sn);
    tcb->setReady(false);
}

void Scheduler::removeTimed(TCB* tcb) {
    timedThreadQueue.removeSTQ(tcb);
    tcb->setSemaphore(nullptr);
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

void Scheduler::updateTimed() {
    while (timedThreadQueue.size() > 0) {
        SleepingNode* sn = timedThreadQueue.popSTQ();
        if (sn->wakeUpTime <= time) {
            sn->tcb->setTimedWaitExpired(true);
            _Semaphore* sem = sn->tcb->getSemaphore();
            sem->removeFromBlockedList(sn->tcb);
            sn->tcb->setSemaphore(nullptr);
            putReady(sn->tcb);
            MemoryAllocator* memAllocator = MemoryAllocator::GetInstance();
            memAllocator->mem_free(sn);
        } else {
            timedThreadQueue.pushSortedSTQ(sn);
            break;
        }
    }
}

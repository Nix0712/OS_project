#include "../h/Scheduler.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/Semaphore.hpp"

LinkedList<TCB> Scheduler::readyThreadQueue;
LinkedList<Scheduler::SleepingNode> Scheduler::waitingThreadQueue;
LinkedList<Scheduler::SleepingNode> Scheduler::sleepingThreadQueue;
uint64 Scheduler::time = 0;

void Scheduler::putReady(TCB* coroutine) {
    readyThreadQueue.insertLast(coroutine);
}

TCB* Scheduler::getReady() {
    TCB* ret = readyThreadQueue.removeFirst();
    return ret;
}

void Scheduler::putSleeping(TCB* tcb, time_t wakeUpTime) {
    SleepingNode* node = new SleepingNode();
    node->tcb = tcb;
    node->wakeUpTime = Scheduler::time + wakeUpTime;
    sleepingThreadQueue.insertSorted(node);
}

void Scheduler::updateSleeping() {
    time++;
    MemoryAllocator* memAllocator = MemoryAllocator::GetInstance();
    SleepingNode* node = sleepingThreadQueue.peakFirst();
    while (node && node->wakeUpTime <= time) {
        TCB* tcb = node->tcb;
        readyThreadQueue.insertLast(tcb);
        memAllocator->mem_free(node);
        sleepingThreadQueue.removeFirst();
        node = sleepingThreadQueue.peakFirst();
    }
}

void Scheduler::putWaiting(TCB* tcb) {
    SleepingNode* node = new SleepingNode();
    node->tcb = tcb;
    _Semaphore* sem = tcb->getSemaphore();
    time_t plusTime = sem->getTimeOut();
    node->wakeUpTime = Scheduler::time + plusTime;
    Scheduler::waitingThreadQueue.insertSorted(node);
}

void Scheduler::updateWaiting() {
    SleepingNode* node = waitingThreadQueue.peakFirst();
    MemoryAllocator* memAllocator = MemoryAllocator::GetInstance();
    while (node && node->wakeUpTime <= time) {
        TCB* tcb = node->tcb;
        _Semaphore* sem = tcb->getSemaphore();
        sem->setTimedOut(true);
        readyThreadQueue.insertLast(tcb);
        memAllocator->mem_free(node);
        sleepingThreadQueue.removeFirst();
        node = sleepingThreadQueue.peakFirst();
    }
}

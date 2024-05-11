#include "../h/Scheduler.hpp"

#include "../h/MemoryAllocator.hpp"

LinkedList<TCB> Scheduler::readyThreadQueue;
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
    node->wakeUpTime = wakeUpTime;
    sleepingThreadQueue.insertLast(node);
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

#include "../h/TCB.hpp"
#include "../h/Scheduler.hpp"

TCB* TCB::running = nullptr;

TCB::TCB(Body body, void* arg, void* stack) : body(body), arg(arg), stack((char*)stack), context({(uint64)&threadWrapper, stack != nullptr ? (uint64) & ((char*)stack)[DEFAULT_STACK_SIZE] : 0}), timeSlice(DEFAULT_TIME_SLICE), finished(false), ready(true), timeSliceCounter(0), closedInSemaphore(false), waitTime(0), semaphore(nullptr), timedWaitExpired(false) {
}

uint64 TCB::getTimeSlice() const {
    return this->timeSlice;
}

bool TCB::isFinished() const {
    return this->finished;
}
void TCB::setFinished(bool finished) {
    this->finished = finished;
}

bool TCB::isReady() const {
    return this->ready;
}

void TCB::setReady(bool ready) {
    this->ready = ready;
}

bool TCB::GetIsClosedInSemaphore() const {
    return this->closedInSemaphore;
}

void TCB::SetIsClosedInSemaphore(bool closedInSemaphore) {
    this->closedInSemaphore = closedInSemaphore;
}

bool TCB::isTimedWaitExpired() const {
    return this->timedWaitExpired;
}
void TCB::setTimedWaitExpired(bool timedWaitExpired) {
    this->timedWaitExpired = timedWaitExpired;
}
uint64 TCB::getWaitTime() const {
    return this->waitTime;
}

void TCB::setWaitTime(uint64 waitTime) {
    this->waitTime = waitTime;
}

_Semaphore* TCB::getSemaphore() const {
    return this->semaphore;
}

void TCB::setSemaphore(_Semaphore* semaphore) {
    this->semaphore = semaphore;
}

void TCB::dispatch() {
    TCB* old = running;
    MemoryAllocator* alloc = MemoryAllocator::GetInstance();
    if (!old->isFinished() && old->isReady()) {
        Scheduler::putReady(old);
    } else if (old->isFinished()) { // After finishing the thread, free the memory of stack
        alloc->mem_free(old->stack);
        alloc->mem_free(old);
    }
    running = Scheduler::getReady();
    if (old != running)
        TCB::contextSwitch(&old->context, &running->context);
}

// Wrapper function for the thread body for calling the function in user mode
void TCB::threadWrapper() {
    RiscV::extract_SSP_SPIE();
    running->body(running->arg);
    thread_exit();
}

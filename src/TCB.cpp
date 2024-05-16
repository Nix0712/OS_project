#include "../h/TCB.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/RiscV.hpp"
#include "../h/Scheduler.hpp"
#include "../h/syscall_c.hpp"

TCB* TCB::running = nullptr;

TCB::TCB(Body body, void* arg, void* stack) : body(body), arg(arg), stack((char*)stack), context({(uint64)&threadWrapper, stack != nullptr ? (uint64) & ((char*)stack)[DEFAULT_STACK_SIZE] : 0}), timeSlice(DEFAULT_TIME_SLICE), finished(false), ready(true), timeSliceCounter(0), closedInSemaphore(false), waitTime(0) {
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

uint64 TCB::getWaitTime() const {
    return this->waitTime;
}

void TCB::setWaitTime(uint64 waitTime) {
    this->waitTime = waitTime;
}

void TCB::dispatch() {
    TCB* old = running;
    MemoryAllocator* alloc = MemoryAllocator::GetInstance();
    if (!old->isFinished() && old->isReady()) {
        Scheduler::putReady(old);
    } else if (old->finished) {
        alloc->mem_free(old->stack);
        alloc->mem_free(old);
    }
    running = Scheduler::getReady();
    if (old != running)
        TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper() {
    RiscV::extract_SSP_SPIE();
    running->body(running->arg);
    thread_exit();
}

#include "../h/TCB.hpp"
#include "../h/RiscV.hpp"
#include "../h/Scheduler.hpp"
#include "../h/syscall_c.hpp"

TCB* TCB::running = nullptr;

TCB::TCB(Body body, void* arg, void* stack) : body(body), arg(arg), stack((char*)stack), context({(uint64)&threadWrapper, stack != nullptr ? (uint64) & ((char*)stack)[DEFAULT_STACK_SIZE] : 0}), timeSlice(2), finished(false), ready(true) {
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

void TCB::dispatch() {
    TCB* old = running;
    if (!old->isFinished() && old->isReady()) {
        Scheduler::putReady(old);
    } else if (old->finished) {
        delete old;
    }
    running = Scheduler::getReady();
    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper() {
    RiscV::extract_SSP_SPIE();
    running->body(running->arg);
    thread_exit();
}

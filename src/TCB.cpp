#include "../h/TCB.hpp"
#include "../h/RiscV.hpp"
#include "../h/Scheduler.hpp"
#include "../h/syscall_c.hpp"

TCB* TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

TCB* TCB::createCoroutine(Body body) {
    return new TCB(body, DEFAULT_TIME_SLICE);
}
TCB::TCB(Body body, uint64 timeSlice) : body(body), stack(new uint64[DEFAULT_STACK_SIZE]), context({(uint64)&threadWrapper, stack != nullptr ? (uint64)&stack[DEFAULT_STACK_SIZE] : 0}), timeSlice(timeSlice), finished(false) {
    Scheduler::putCoroutine(this);
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

void TCB::dispatch() {
    TCB* old = running;
    if (!old->isFinished()) {
        Scheduler::putCoroutine(old);
    }
    running = Scheduler::getCoroutine();

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper() {
    RiscV::extract_SSP_SPIE();
    running->body();
    thread_exit();
}

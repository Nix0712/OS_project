#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/console.h"
#include "../h/syscall_cpp.hpp"

extern void userMain();

void userMainWrapper(void* arg) {
    // Semaphore* sem = (Semaphore*)arg;
    userMain();
    // sem->signal();
}

void idle(void* arg) {
    while (true) {
        Thread::dispatch();
    }
}

int main() {
    RiscV::write_stvec((uint64)&RiscV::supervisorTrap);

    TCB::running = new TCB(nullptr, nullptr, nullptr);

    Thread* idleThread = new Thread(idle, nullptr);

    Semaphore* sem = new Semaphore(0);

    Thread* userThread = new Thread(userMainWrapper, nullptr);

    idleThread->start();
    userThread->start();

    while (true) {
        Thread::dispatch();
    }
    delete sem;
    return 0;
}
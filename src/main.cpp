#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/console.h"
#include "../h/syscall_cpp.hpp"

extern void userMain();

void userMainWrapper(void* arg) {
    Semaphore* sem = (Semaphore*)arg;
    userMain();
    sem->signal();
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
    idleThread->start();

    Thread* userThread = new Thread(userMainWrapper, sem);
    userThread->start();

    sem->wait();

    delete sem;

    return 0;
}
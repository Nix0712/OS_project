#include "../h/syscall_cpp.hpp"

void* operator new(size_t size) { return mem_alloc(size); }

void* operator new[](size_t size) { return mem_alloc(size); }

void operator delete(void* addr) { mem_free(addr); }

void operator delete[](void* addr) { mem_free(addr); }

Thread::Thread(void (*body)(void*), void* arg) : myHandle(0), body(body), arg(arg) {}
Thread::Thread() : myHandle(0), body(runWrapper), arg(this) {}

int Thread::start() {
    return thread_create(&myHandle, body, arg);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

Thread::~Thread() {
}

Semaphore::Semaphore(unsigned init) : myHandle(0) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::timedWait(time_t time) {
    return sem_timedwait(myHandle, time);
}

int Semaphore::tryWait() {
    return sem_trywait(myHandle);
}

PeriodicThread::PeriodicThread(time_t period) : Thread(), period(period) {}

void PeriodicThread::terminate() {
    period = 0;
}

void PeriodicThread::run() {
    while (period) {
        periodicActivation();
        sleep(period);
    }
}
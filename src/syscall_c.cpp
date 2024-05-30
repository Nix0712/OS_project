#include "../h/syscall_c.hpp"

void* mem_alloc(size_t size) {
    size_t num_of_blocks = ((size + MEM_BLOCK_SIZE - 1) + 24UL) / MEM_BLOCK_SIZE;

    __asm__ volatile("mv a1, %[num_of_blocks]" : : [num_of_blocks] "r"(num_of_blocks));
    __asm__ volatile("mv a0, %0" : : "r"(MEM_ALLOC));

    __asm__ volatile("ecall");

    void* volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}
int mem_free(void* addr) {
    __asm__ volatile("mv a1, %[addr]" : : [addr] "r"(addr));
    __asm__ volatile("mv a0, %0" : : "r"(MEM_FREE));

    __asm__ volatile("ecall");

    size_t volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));

    return ret;
}

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg) {
    void* stack = mem_alloc(DEFAULT_STACK_SIZE);
    if (stack == nullptr) {
        return -1;
    }
    __asm__ volatile("mv a4, %[stack]" : : [stack] "r"(stack));
    __asm__ volatile("mv a3, %0" : : "r"(arg));
    __asm__ volatile("mv a2, %0" : : "r"(start_routine));
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("mv a0, %0" : : "r"(THREAD_CREATE));

    __asm__ volatile("ecall");

    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

int thread_exit() {
    __asm__ volatile("mv a0, %0" : : "r"(THREAD_EXIT));

    __asm__ volatile("ecall");

    return 0;
}

void thread_dispatch() {
    __asm__ volatile("mv a0, %0" : : "r"(THREAD_DISPATCH));

    __asm__ volatile("ecall");
}

int sem_open(sem_t* handle, unsigned init) {
    __asm__ volatile("mv a2, %0" : : "r"(init));
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("mv a0, %0" : : "r"(SEM_OPEN));

    __asm__ volatile("ecall");

    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

int sem_close(sem_t handle) {
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("mv a0, %0" : : "r"(SEM_CLOSE));

    __asm__ volatile("ecall");

    return 0;
}

int sem_wait(sem_t id) {
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("mv a0, %0" : : "r"(SEM_WAIT));

    __asm__ volatile("ecall");

    return 0;
}

int sem_signal(sem_t id) {
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("mv a0, %0" : : "r"(SEM_SIGNAL));

    __asm__ volatile("ecall");

    return 0;
}

int sem_timedwait(sem_t id, time_t timeout) {
    __asm__ volatile("mv a2, %0" : : "r"(timeout));
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("mv a0, %0" : : "r"(SEM_TIMEDWAIT));

    __asm__ volatile("ecall");

    int volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

int sem_trywait(sem_t id) {
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("mv a0, %0" : : "r"(SEM_TRYWAIT));

    __asm__ volatile("ecall");

    return 0;
}

int time_sleep(time_t time) {
    __asm__ volatile("mv a1, %0" : : "r"(time));
    __asm__ volatile("mv a0, %0" : : "r"(TIME_SLEEP));

    __asm__ volatile("ecall");

    return 0;
}

char getc() {
    __asm__ volatile("mv a0, %0" : : "r"(GETC));

    __asm__ volatile("ecall");

    char volatile ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

void putc(char c) {
    __asm__ volatile("mv a1, %0" : : "r"(c));
    __asm__ volatile("mv a0, %0" : : "r"(PUTC));

    __asm__ volatile("ecall");
}
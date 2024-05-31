#pragma once

#include "../lib/hw.h"

enum calls {
    MEM_ALLOC = 0x01,
    MEM_FREE,
    THREAD_CREATE = 0x11,
    THREAD_EXIT,
    THREAD_DISPATCH,
    SEM_OPEN = 0x21,
    SEM_CLOSE,
    SEM_WAIT,
    SEM_SIGNAL,
    SEM_TIMEDWAIT,
    SEM_TRYWAIT,
    TIME_SLEEP = 0x31,
    GETC = 0x41,
    PUTC
};

// size_t num_of_blocks = ((byte_size + MEM_BLOCK_SIZE - 1) + sizeof(AllocMemeBlocks)) / MEM_BLOCK_SIZE;
void* mem_alloc(size_t size);

int mem_free(void* addr);

// Thread class
class _thread;
typedef _thread* thread_t;

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg);

int thread_exit();
void thread_dispatch();

// Semaphore class
class _sem;
typedef _sem* sem_t;

int sem_open(sem_t* handle, unsigned init);

int sem_close(sem_t handle);
int sem_wait(sem_t id);
int sem_signal(sem_t id);
int sem_timedwait(sem_t id, time_t timeout);
int sem_trywait(sem_t id);

typedef unsigned long time_t;
int time_sleep(time_t);

const int EOF = -1;
char getc();
void putc(char);

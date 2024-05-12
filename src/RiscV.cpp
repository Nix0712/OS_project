#include "../h/RiscV.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/Scheduler.hpp"
#include "../h/Semaphore.hpp"
#include "../h/TCB.hpp"
#include "../h/console.h"
#include "../h/syscall_c.hpp"

void RiscV::supervisorTrapHandler() {
    volatile uint64 scause = read_scause();
    volatile uint64 sepc = read_sepc();
    volatile uint64 sstatus = read_sstatus();

    // Handle the trap
    switch (scause) {
    case 0x8000000000000001UL: { // Supervisor software interrupt (timer)
        Scheduler::updateSleeping();
        Scheduler::updateWaiting();

        TCB::running->timeSliceCounter++;
        if (TCB::running->timeSliceCounter >= TCB::running->getTimeSlice()) {
            TCB::running->timeSliceCounter = 0;
            TCB::dispatch();
            write_sstatus(sstatus);
            write_sepc(sepc);
        }

        clear_mask_sip(SIP_SSIE);
        break;
    }
    case 0x8000000000000009UL: { // Supervisor external interrupt (console)
        console_handler();
        break;
    }

    case 0x0000000000000008UL: // U-mode ecall
    case 0x0000000000000009UL: // S-mode ecall
    {
        sepc += 4;
        uint64 a0 = read_reg(10); // System call number
        switch (a0) {
        case MEM_ALLOC: {
            uint64 a1 = read_reg(11);
            void* ret;
            MemoryAllocator* memAllocator = MemoryAllocator::GetInstance();
            ret = memAllocator->mem_alloc((uint64)a1);
            write_reg(10, (uint64)ret);
            break;
        }
        case MEM_FREE: {
            void* a1 = (void*)read_reg(11);
            MemoryAllocator* memAllocator = MemoryAllocator::GetInstance();
            size_t retVal = memAllocator->mem_free(a1);
            write_reg(10, retVal);
            break;
        }
        case THREAD_CREATE: {
            thread_t* a1 = (thread_t*)read_reg(11);
            void (*a2)(void*) = (void (*)(void*))read_reg(12);
            void* a3 = (void*)read_reg(13);
            void* a4 = (void*)read_reg(14);
            TCB* thread = new TCB(a2, a3, a4);
            *(TCB**)a1 = thread;
            if (thread != nullptr) {
                Scheduler::putReady(thread);
                write_reg(10, 0);
            } else {
                write_reg(10, -1);
            }
            break;
        }

        case THREAD_EXIT: {
            TCB::running->setFinished(true);
            TCB::dispatch();
            break;
        }

        case THREAD_DISPATCH: {
            TCB::dispatch();
            break;
        }

        case SEM_OPEN: {
            _Semaphore** a1 = (_Semaphore**)read_reg(11);
            unsigned a2 = (unsigned)read_reg(12);
            *a1 = new _Semaphore(a2);
            TCB::running->sem = *a1;
            TCB::running->setWorkingSemaphore(1);
            if (*a1 != nullptr)
                write_reg(10, 0);
            else
                write_reg(10, -1);
            break;
        }

        case SEM_CLOSE: {
            _Semaphore* a1 = (_Semaphore*)read_reg(11);
            a1->~_Semaphore();
            TCB::running->setWorkingSemaphore(-1);
            delete a1;
            break;
        }

        case SEM_WAIT: {
            _Semaphore* a1 = (_Semaphore*)read_reg(11);
            if (!a1 || TCB::running->getWorkingSemaphore() == -1) {
                write_reg(10, -1);
                break;
            }
            a1->wait();
            write_reg(10, 0);
            break;
        }

        case SEM_SIGNAL: {
            _Semaphore* a1 = (_Semaphore*)read_reg(11);
            if (!a1) {
                write_reg(10, -1);
                break;
            }
            a1->signal();
            write_reg(10, 0);
            break;
        }

        case SEM_TIMEDWAIT: {
            _Semaphore* a1 = (_Semaphore*)read_reg(11);
            time_t a2 = (time_t)read_reg(12);
            if (!a1) {
                write_reg(10, -1);
                break;
            }
            a1->timedwait(a2);
            if (a1->isTimedOut())
                write_reg(10, -2);
            else
                write_reg(10, 0);
            break;
        }

        case SEM_TRYWAIT: {
            _Semaphore* a1 = (_Semaphore*)read_reg(11);
            if (!a1) {
                write_reg(10, -1);
                break;
            }
            a1->trywait();
            if (a1->getIsBlockedOnTry())
                write_reg(10, 0);
            else
                write_reg(10, 1);
            break;
        }

        case TIME_SLEEP: {
            time_t a1 = (time_t)read_reg(11);
            TCB::running->setReady(false);
            Scheduler::putSleeping(TCB::running, a1);
            TCB::dispatch();
            write_reg(10, 0);
            break;
        }

        case GETC: {
            char retVal = __getc();
            write_reg(10, retVal);
            break;
        }

        case PUTC: {
            char a1 = (char)read_reg(11);
            __putc(a1);
            break;
        }
        default:
            break;
        }
    }

    default:
        // Unexpected trap
        // print scause, sepc, stval
        break;
    }

    // Return from trap
    write_sepc(sepc);
}

void RiscV::extract_SSP_SPIE() {
    __asm__ volatile("csrc sstatus, %0" ::"r"(SSTATUS_SPP));
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}
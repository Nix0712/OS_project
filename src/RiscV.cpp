#include "../h/RiscV.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/Scheduler.hpp"
#include "../h/Semaphore.hpp"
#include "../h/TCB.hpp"
#include "../h/console.h"
#include "../h/syscall_c.hpp"

void RiscV::supervisorTrapHandler() {
    uint64 scause = read_scause();
    uint64 sepc = read_sepc();

    // Handle the trap
    switch (scause) {
    case 0x8000000000000001UL: { // Supervisor software interrupt (timer)
        Scheduler::updateSleeping();

        TCB::running->timeSliceCounter++;
        if (TCB::running->timeSliceCounter >= TCB::running->getTimeSlice()) {
            volatile uint64 sepc = read_sepc();
            volatile uint64 sstatus = read_sstatus();
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
            // TODO:
            break;
        }

        case SEM_CLOSE: {
            // TODO:
            break;
        }

        case SEM_WAIT: {
            // TODO:
            break;
        }

        case SEM_SIGNAL: {
            // TODO:
            break;
        }

        case SEM_TIMEDWAIT: {
            // TODO:
            break;
        }

        case SEM_TRYWAIT: {
            // TODO:
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
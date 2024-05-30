#include "../h/RiscV.hpp"
#include "../h/Console.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/Scheduler.hpp"
#include "../h/Semaphore.hpp"
#include "../h/TCB.hpp"
#include "../h/syscall_c.hpp"

// Trap handler for syscalls and interrupts in supervisor mode
void RiscV::supervisorTrapHandler() {
    volatile uint64 scause = read_scause();
    volatile uint64 sepc = read_sepc();
    volatile uint64 sstatus = read_sstatus();

    // Handle the trap
    switch (scause) {
    case 0x8000000000000001UL: { // Supervisor software interrupt (timer)
        clear_mask_sip(SIP_SSIE);
        Scheduler::updateTime();
        Scheduler::updateSleeping();
        Scheduler::updateTimed();

        TCB::running->timeSliceCounter++;
        if (TCB::running->timeSliceCounter >= TCB::running->getTimeSlice()) {
            TCB::running->timeSliceCounter = 0;
            TCB::dispatch();
        }
        break;
    }
    case 0x8000000000000009UL: { // Supervisor external interrupt (console)
        int a = plic_claim();
        if (a == 10) {
            _Console* console = _Console::GetInstance();
            console->console_handler();
        }
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
            _Semaphore* sem = new _Semaphore(a2);
            *a1 = sem;
            write_reg(10, 0);
            break;
        }

        case SEM_CLOSE: {
            _Semaphore* a1 = (_Semaphore*)read_reg(11);
            a1->~_Semaphore();
            MemoryAllocator* memAllocator = MemoryAllocator::GetInstance();
            size_t retVal = memAllocator->mem_free(a1);
            if (retVal == 0)
                write_reg(10, 0);
            else
                write_reg(10, -1);
            break;
        }

        case SEM_WAIT: {
            _Semaphore* a1 = (_Semaphore*)read_reg(11);
            if (!a1) {
                write_reg(10, -1);
                break;
            }
            int ret = a1->wait();
            write_reg(10, ret);
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
            if (!a1) {
                write_reg(10, -3);
                break;
            }
            time_t a2 = (time_t)read_reg(12);
            int ret = a1->timedwait(a2);
            write_reg(10, ret);
            break;
        }

        case SEM_TRYWAIT: {
            _Semaphore* a1 = (_Semaphore*)read_reg(11);
            if (a1) {
                int ret = a1->trywait();
                write_reg(10, ret);
            } else {
                write_reg(10, -1);
            }
            break;
        }

        case TIME_SLEEP: {
            time_t a1 = (time_t)read_reg(11);
            Scheduler::putSleeping(TCB::running, a1);
            TCB::dispatch();
            write_reg(10, 0);
            break;
        }

        case GETC: {
            _Console* console = _Console::GetInstance();
            char retVal = console->_getc();
            write_reg(10, retVal);
            break;
        }

        case PUTC: {
            char a1 = (char)read_reg(11);
            _Console* console = _Console::GetInstance();
            console->_putc(a1);
            break;
        }
        default:
            break;
        }
        break;
    }
    case 0x0000000000000002UL: { // U-mode illegal instruction
        uint32 val = 0x5555;
        uint64 addr = 0x100000;
        __asm__ volatile("sw %[val], 0(%[addr])" : : [val] "r"(val), [addr] "r"(addr));
        break;
    }
    default:
        uint32 val = 0x5555;
        uint64 addr = 0x100000;
        __asm__ volatile("sw %[val], 0(%[addr])" : : [val] "r"(val), [addr] "r"(addr));
        break;
    }

    // Return from trap
    write_sstatus(sstatus);
    write_sepc(sepc);
}

// Change priviledge level to unprivileged and jump to user mode (and enables interrupts )
void RiscV::extract_SSP_SPIE() {
    __asm__ volatile("csrc sstatus, %0" ::"r"(SSTATUS_SPP));
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}
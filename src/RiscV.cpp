#include "../h/RiscV.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/TCB.hpp"
#include "../h/console.h"
#include "../h/syscall_c.hpp"

void RiscV::supervisorTrapHandler() {
    uint64 scause = read_scause();
    uint64 sepc = read_sepc();

    // Handle the trap
    switch (scause) {
    case 0x8000000000000001UL: { // Supervisor software interrupt (timer)
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
            uint64 sepc = read_sepc();
            uint64 sstatus = read_sstatus();
            TCB::timeSliceCounter = 0;
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
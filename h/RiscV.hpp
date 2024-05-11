#pragma once

// RiscV interface
#include "../h/hw.h"

/*
    Register description:
        stvec -> that holds trap vector configuration (Supervisor Trap Vector Base Address Register)
        scause -> register that holds information of event that cased the trap (Supervisor Cause Register)
        sepc -> contains virtual address of the instruction that was interrupted or that encountered the exception (Supervisor Exception Program Counter)
        stval -> written with exception-specific information to assist software in handling the trap (Supervisor Trap Value)
        sip -> containing information on pending interrupts (Supervisor Interrupt Registers)
        sstatus -> keeps track of the processor’s current operating state (Supervisor Status Register)

    Mask for SIE, SPIE and SSP are applied to value in sstatus register
        SIE -> enables or disables all interrupts in supervisor mode.
        SPIE -> indicates whether supervisor interrupts were enabled prior to trapping into supervisor mode
        SSP -> indicates the privilege level at which a hart was executing before entering supervisor mode

    Mask for SSIE, STIE and SEIE are applied to value in sip register
        SSIE -> interrupt-enable bits for supervisor-level software interrupts
        STIE -> interrupt-enable bits for supervisor-level timer interrupts
        SEIE -> interrupt-enable bits for supervisor-level external interrupts
*/

class RiscV {
  public:
    // Enum made to get masks for SIE, SPIE and SPP bits inside of sstatus register
    enum SstatusBitMask {
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8),
    };

    // Enum made to get masks for bit for SSIE, STIE and SEIE inside of sip register
    enum SipBitMask {
        SIP_SSIE = (1 << 1),
        SIP_STIE = (1 << 5),
        SIP_SEIE = (1 << 9),
    };

    void static extract_SSP_SPIE();

    static void saveRegisters();
    static void restoreRegisters();

    // Reading and writing for registers
    static uint64 read_stvec();
    static void write_stvec(uint64 stvec);

    static uint64 read_scause();
    static void write_scause(uint64 scause);

    static uint64 read_sepc();
    static void write_sepc(uint64 sepc);

    static uint64 read_stval();
    static void write_stval(uint64 stval);

    static uint64 read_sip();
    static void write_sip(uint64 sip);

    static uint64 read_sstatus();
    static void write_sstatus(uint64 sstautus);

    // Set and clear for registers
    static void set_mask_sip(uint64 mask);
    static void clear_mask_sip(uint64 mask);

    static void set_mask_sstatus(uint64 mask);
    static void clear_mask_sstatus(uint64 mask);

    static uint64 read_reg(uint64 reg);
    static void write_reg(uint64 reg, uint64 value);

    static void supervisorTrap();

  private:
    static void supervisorTrapHandler();
};

inline uint64 RiscV::read_stvec() {
    uint64 volatile stvec;
    __asm__ volatile("csrr %[stvec], stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void RiscV::write_stvec(uint64 stvec) {
    __asm__ volatile("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}

inline uint64 RiscV::read_scause() {
    uint64 volatile scause;
    __asm__ volatile("csrr %[scause], scause" : [scause] "=r"(scause));
    return scause;
}

inline void RiscV::write_scause(uint64 scause) {
    __asm__ volatile("csrw scause, %[scause]" : : [scause] "r"(scause));
}

inline uint64 RiscV::read_sepc() {
    uint64 volatile sepc;
    __asm__ volatile("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void RiscV::write_sepc(uint64 sepc) {
    __asm__ volatile("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline uint64 RiscV::read_stval() {
    uint64 volatile stval;
    __asm__ volatile("csrr %[stval], stval" : [stval] "=r"(stval));
    return stval;
}

inline void RiscV::write_stval(uint64 stval) {
    __asm__ volatile("csrw stval, %[stval]" : : [stval] "r"(stval));
}

inline uint64 RiscV::read_sip() {
    uint64 volatile sip;
    __asm__ volatile("csrr %[sip], sip" : [sip] "=r"(sip));
    return sip;
}

inline void RiscV::write_sip(uint64 sip) {
    __asm__ volatile("csrw sip, %[sip]" : : [sip] "r"(sip));
}

inline uint64 RiscV::read_sstatus() {
    uint64 volatile sstatus;
    __asm__ volatile("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void RiscV::write_sstatus(uint64 sstatus) {
    __asm__ volatile("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

inline void RiscV::set_mask_sip(uint64 mask) {
    __asm__ volatile("csrs sip, %[mask]" : : [mask] "r"(mask));
}

inline void RiscV::clear_mask_sip(uint64 mask) {
    __asm__ volatile("csrc sip, %[mask]" : : [mask] "r"(mask));
}

inline void RiscV::set_mask_sstatus(uint64 mask) {
    __asm__ volatile("csrs sstatus, %[mask]" : : [mask] "r"(mask));
}

inline void RiscV::clear_mask_sstatus(uint64 mask) {
    __asm__ volatile("csrc sstatus, %[mask]" : : [mask] "r"(mask));
}

inline uint64 RiscV::read_reg(uint64 reg) {
    uint64 volatile value;
    __asm__ volatile("ld %[value], 8*%[reg](fp)" : [value] "=r"(value) : [reg] "n"(reg));
    return value;
}

inline void RiscV::write_reg(uint64 reg, uint64 value) {
    __asm__ volatile("sd %[value], 8*%[reg](fp)" : : [value] "r"(value), [reg] "n"(reg));
}

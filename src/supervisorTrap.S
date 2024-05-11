    .extern _ZN5RiscV21supervisorTrapHandlerEv

    .align  4
    .global _ZN5RiscV14supervisorTrapEv
    .type   _ZN5RiscV14supervisorTrapEv, @function
_ZN5RiscV14supervisorTrapEv:
#Move stack pointer to 32 places for 32 registers (32 registers * 8B = 256B)
    addi    sp, sp, -256

#Push registers to stack
    .irp    index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd      x\index, \index * 8(sp)
    .endr
    addi    fp, sp, 256

    call    _ZN5RiscV21supervisorTrapHandlerEv

#Pop registers from stack
    .irp    index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld      x\index, \index * 8(sp)
    .endr
    addi    sp, sp, 256
    sret
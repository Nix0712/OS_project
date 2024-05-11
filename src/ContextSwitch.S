    .global _ZN3TCB13contextSwitchEPNS_7ContextES1_
    .type   _ZN3TCB13contextSwitchEPNS_7ContextES1_, @function
_ZN3TCB13contextSwitchEPNS_7ContextES1_:
#Inside of a0 is &old->context and inside of a1 is &running->context

    sd      ra, 0(a0)
    sd      sp, 8(a0)

    sd      ra, 0(a1)
    sd      sp, 8(a1)

    ret

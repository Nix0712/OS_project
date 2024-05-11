#include "../h/RiscV.hpp"
#include "../h/console.h"
#include "../h/syscall_c.hpp"

int main() {

    RiscV::write_stvec((uint64)&RiscV::supervisorTrap);
    void* probaj = mem_alloc(100);

    mem_free(probaj);
    return 0;
}
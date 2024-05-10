#include "../h/syscall_cpp.hpp"
#include "../h/mem.h"

void* operator new(size_t size) { return __mem_alloc(size); }

void operator delete(void* addr) { __mem_free(addr); }

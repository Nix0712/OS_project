#pragma once

#include "./hw.h"

class MemoryAllocator {
  private:
    // LinkedList for free memory blocks
    struct FreeMemBlock {
        size_t MemSizeBlock;
        FreeMemBlock* prev;
        FreeMemBlock* next;
    };

    FreeMemBlock* head;
    FreeMemBlock* lastAllocated;

    MemoryAllocator() {
        head = (FreeMemBlock*)MEM_BLOCK_SIZE;
        head->next = nullptr;
        head->prev = nullptr;
        lastAllocated = nullptr;
    }

    static MemoryAllocator* memallocator_;

  public:
    void* mem_alloc(size_t byte_size);
    size_t mem_free(void* addr);

    void joinFragments(FreeMemBlock* begin, FreeMemBlock* end);

    MemoryAllocator(MemoryAllocator& other) = delete;
    void operator=(const FreeMemBlock&) = delete;

    static MemoryAllocator* GetInstance();
};
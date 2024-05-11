#pragma once

#include "./hw.h"

class MemoryAllocator {
  private:
    // LinkedList for free memory blocks
    struct AllocMemBlocks {
        size_t BlockNum;
        AllocMemBlocks* next;
        AllocMemBlocks* prev;
    };

    AllocMemBlocks* head;

    MemoryAllocator() {
        head = nullptr;
        // head->BlockNum = 0;
        // head->next = nullptr;
        // head->prev = nullptr;
    }

  public:
    void* mem_alloc(size_t num_of_blocks);
    size_t mem_free(void* addr);

    MemoryAllocator(MemoryAllocator& other) = delete;
    void operator=(const AllocMemBlocks&) = delete;

    static MemoryAllocator* GetInstance();
};
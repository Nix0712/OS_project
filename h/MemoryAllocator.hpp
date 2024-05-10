#pragma once

#include "./hw.h"

class MemoryAllocator {
  private:
    // LinkedList for free memory blocks
    struct AllocMemeBlocks {
        size_t BlockNum;
        AllocMemeBlocks* next;
        AllocMemeBlocks* prev;
    };

    AllocMemeBlocks* head;

    MemoryAllocator() {
        head = nullptr;
        head->BlockNum = 0;
        head->next = nullptr;
        head->prev = nullptr;
    }

  public:
    void* mem_alloc(size_t byte_size);
    size_t mem_free(void* addr);

    MemoryAllocator(MemoryAllocator& other) = delete;
    void operator=(const AllocMemeBlocks&) = delete;

    static MemoryAllocator* GetInstance();
};
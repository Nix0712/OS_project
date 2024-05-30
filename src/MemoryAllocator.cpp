#include "../h/MemoryAllocator.hpp"
#include "../h/mem.h"

// Returning instance of created MemoryAllocator (Singlton)
MemoryAllocator* MemoryAllocator::GetInstance() {
    static MemoryAllocator memallocator_;
    return &memallocator_;
}

// Memory allocation for given number of blokcs
void* MemoryAllocator::mem_alloc(size_t num_of_blocks) {
    // return __mem_alloc(num_of_blocks);
    num_of_blocks = ((num_of_blocks + MEM_BLOCK_SIZE - 1) + sizeof(AllocMemBlocks)) / MEM_BLOCK_SIZE;
    size_t freeBlocks;

    // Initial allocation for header Node
    if (head == nullptr) {
        freeBlocks = ((size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR) / MEM_BLOCK_SIZE;
        if (freeBlocks < num_of_blocks)
            return nullptr;

        head = (AllocMemBlocks*)HEAP_START_ADDR;
        head->BlockNum = num_of_blocks;
        head->next = nullptr;
        head->prev = nullptr;
        return (void*)((size_t)head + sizeof(AllocMemBlocks));
    }

    // Searching for free space and allocating required space
    AllocMemBlocks* curr = head;
    bool headNode = true;

    while (curr) {
        void* startAddress;
        void* endAddress;

        // If our current position is the head Node, it's essential to verify whether there is any available space before the head
        if (headNode && curr->prev == nullptr) {
            startAddress = (void*)HEAP_START_ADDR;
            endAddress = (void*)curr;
        } else {
            startAddress = (void*)((uint64)curr + (curr->BlockNum * MEM_BLOCK_SIZE));
            endAddress = (curr->next == nullptr) ? (void*)(HEAP_END_ADDR) : (void*)(curr->next);
        }
        freeBlocks = ((size_t)endAddress - (size_t)startAddress) / MEM_BLOCK_SIZE;

        // Check if there is spece before first node (if head node doesn't start from the beginning)
        if (freeBlocks >= num_of_blocks) {

            AllocMemBlocks* newNode = (AllocMemBlocks*)startAddress;
            newNode->BlockNum = num_of_blocks;

            // Inserting into linked list for allocated space, depending on whether it's before or after the head Node.
            if (headNode) {
                newNode->next = curr;
                newNode->prev = nullptr;
                curr->prev = newNode;
                head = newNode;
            } else {
                newNode->next = curr->next;
                newNode->prev = curr;
                curr->next = newNode;
                if (newNode->next != nullptr)
                    newNode->next->prev = newNode;
            }

            return (void*)((size_t)newNode + sizeof(AllocMemBlocks));
        }

        // Making sure we've fully checked the space around the head Node
        if (headNode) {
            headNode = false;
            continue;
        }
        curr = curr->next;
    }
    return nullptr;
}

size_t MemoryAllocator::mem_free(void* addr) {
    // return __mem_free(addr);
    AllocMemBlocks* curr = (AllocMemBlocks*)((size_t)addr - sizeof(AllocMemBlocks));
    if (curr == nullptr)
        return -1;
    if (curr == head) {
        head = curr->next;
        if (head != nullptr)
            head->prev = nullptr;
        return 0;
    }
    // cheack if it's valid addr
    curr->prev->next = curr->next;
    if (curr->next != nullptr)
        curr->next->prev = curr->prev;

    return 0;
}

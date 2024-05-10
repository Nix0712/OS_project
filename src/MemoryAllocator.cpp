#include "../h/MemoryAllocator.hpp"

// Returning instance of created MemoryAllocator (Singlton)
MemoryAllocator* MemoryAllocator::GetInstance() {
    static MemoryAllocator memallocator_;
    return &memallocator_;
}

// Memory allocation for given number of bytes
void* MemoryAllocator::mem_alloc(size_t byte_size) {

    // Calculating space required for one Node of MemoryAllocator and the space needed for allocation
    size_t num_of_blocks = ((byte_size + MEM_BLOCK_SIZE - 1) + sizeof(AllocMemeBlocks)) / MEM_BLOCK_SIZE;

    size_t freeBlocks;

    // Initial allocation for header Node
    if (head == nullptr) {
        freeBlocks = ((size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR) / MEM_BLOCK_SIZE;
        if (freeBlocks < num_of_blocks)
            return nullptr;

        head = (AllocMemeBlocks*)HEAP_START_ADDR;
        head->BlockNum = num_of_blocks;
        return (void*)((size_t)head + sizeof(AllocMemeBlocks));
    }

    // Searching for free space and allocating required space
    AllocMemeBlocks* curr = head;
    bool headNode = true;

    while (curr) {
        void* startAddress;
        void* endAddress;

        // If our current position is the head Node, it's essential to verify whether there is any available space before the head
        if (curr->prev == nullptr && headNode) {
            startAddress = (void*)HEAP_START_ADDR;
            endAddress = (void*)curr;
        } else {
            startAddress = (void*)(curr + (curr->BlockNum * MEM_BLOCK_SIZE));
            endAddress = (curr->next == nullptr) ? (void*)(HEAP_END_ADDR) : (void*)(curr->next);
        }
        freeBlocks = ((size_t)endAddress - (size_t)startAddress) / MEM_BLOCK_SIZE;

        // Check if there is spece before first node (if head node doesn't start from the beginning)
        if (freeBlocks >= num_of_blocks) {

            AllocMemeBlocks* newNode = (AllocMemeBlocks*)startAddress;
            newNode->BlockNum = num_of_blocks;

            // Inserting into linked list for allocated space, depending on whether it's before or after the head Node.
            if (headNode) {
                newNode->next = curr;
                newNode->prev = nullptr;
                curr->prev = newNode;
            } else {
                newNode->next = curr->next;
                newNode->prev = curr;
                curr->next = newNode;
                if (newNode->next != nullptr)
                    newNode->next->prev = newNode;
            }

            return (void*)((size_t)newNode + sizeof(AllocMemeBlocks));
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
    AllocMemeBlocks* curr = (AllocMemeBlocks*)((size_t)addr - sizeof(AllocMemeBlocks));
    if (curr == nullptr)
        return -1;
    if (curr == head) {
        head = curr->next;
        return 0;
    }

    curr->prev->next = curr->next;
    if (curr->next != nullptr)
        curr->next->prev = curr->prev;

    return 0;
}

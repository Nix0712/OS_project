#include "../h/MemoryAllocator.hpp"

MemoryAllocator* MemoryAllocator::memallocator_ = nullptr;

MemoryAllocator* MemoryAllocator::GetInstance() {
    if (memallocator_ == nullptr)
        memallocator_ = new MemoryAllocator();
    return memallocator_;
}

void* MemoryAllocator::mem_alloc(size_t byte_size) {
    size_t num_of_blocks = (byte_size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    return nullptr;
}

#include "../h/List.hpp"
#include "../h/MemoryAllocator.hpp"

// Push TCB to the queue
void Queue::pushTCB(TCB* data) {
    size_t numOfBlocks = ((sizeof(Node) + MEM_BLOCK_SIZE - 1) + 24UL) / MEM_BLOCK_SIZE;
    Node* newNode = (Node*)MemoryAllocator::GetInstance()->mem_alloc(numOfBlocks);
    newNode->data = (void*)data;
    newNode->next = nullptr;
    if (head == nullptr) {
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    length++;
}

// Push SleepingNode to the queue
void Queue::pushSortedSTQ(SleepingNode* data) {
    // Insert in sorted order but witout carrying about the tail so tail will be nullptr
    size_t numOfBlocks = ((sizeof(Node) + MEM_BLOCK_SIZE - 1) + 24UL) / MEM_BLOCK_SIZE;
    Node* newNode = (Node*)MemoryAllocator::GetInstance()->mem_alloc(numOfBlocks);
    newNode->data = data;
    newNode->next = nullptr;

    if (head == nullptr) {
        head = newNode;
        length++;
        return;
    }

    Node* current = head;
    Node* previous = nullptr;

    while (current && ((SleepingNode*)current->data)->wakeUpTime < data->wakeUpTime) {
        previous = current;
        current = current->next;
    }
    if (previous == nullptr) {
        newNode->next = head;
        head = newNode;
    } else {
        previous->next = newNode;
        newNode->next = current;
    }
    length++;
}

// Pop TCB from the queue
TCB* Queue::popTCB() {
    MemoryAllocator* alloc = MemoryAllocator::GetInstance();
    if (head == nullptr)
        return nullptr;
    Node* temp = head;
    head = head->next;
    TCB* tcb = (TCB*)temp->data;
    if (head == nullptr)
        tail = nullptr;
    alloc->mem_free(temp);
    length--;
    return tcb;
}

// Pop SleepingNode from the queue
SleepingNode* Queue::popSTQ() {
    MemoryAllocator* alloc = MemoryAllocator::GetInstance();
    if (head == nullptr)
        return nullptr;
    Node* temp = head;
    head = head->next;
    SleepingNode* sleepingNode = (SleepingNode*)temp->data;
    if (head == nullptr)
        tail = nullptr;
    alloc->mem_free(temp);
    length--;
    return sleepingNode;
}

// get data from the front of the queue
void* Queue::front() {
    if (head == nullptr)
        return nullptr;
    return head->data;
}

// get data from the back of the queue
void* Queue::back() {
    if (tail == nullptr)
        return nullptr;
    return tail->data;
}

// get queue size
int Queue::size() {
    return length;
}

// check if queue is empty
bool Queue::empty() {
    if (length == 0)
        return true;
    return false;
}

// remove TCB from queue, this is used to remove TCB from semaphore blocked queue
void Queue::removeTCB(TCB* tcb) {
    Node* current = head;
    Node* previous = nullptr;
    while (current) {
        if (current->data == (void*)tcb) {
            if (previous == nullptr) {
                head = current->next;
            } else {
                previous->next = current->next;
            }
            if (current == tail) {
                tail = previous;
            }
            MemoryAllocator* alloc = MemoryAllocator::GetInstance();
            alloc->mem_free(current);
            length--;
            return;
        }
        previous = current;
        current = current->next;
    }
}

// In case of timedWait(), removing TCB from timedThreadQueue
void Queue::removeSTQ(TCB* sn) {
    Node* current = head;
    Node* previous = nullptr;
    while (current) {
        if (((SleepingNode*)current->data)->tcb == sn) {
            if (previous == nullptr) {
                head = current->next;
            } else {
                previous->next = current->next;
            }
            MemoryAllocator* alloc = MemoryAllocator::GetInstance();
            alloc->mem_free(current);
            length--;
            return;
        }
        previous = current;
        current = current->next;
    }
}

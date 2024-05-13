#include "../h/List.hpp"

void Queue::pushTCB(TCB* data) {
    Node* newNode = new Node;
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

void Queue::pushSortedSTQ(SleepingNode* data) {
    // Insert in sorted order but witout carrying about the tail so tail will be nullptr
    Node* newNode = new Node;
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

TCB* Queue::popTCB() {
    if (head == nullptr)
        return nullptr;
    Node* temp = head;
    head = head->next;
    TCB* tcb = (TCB*)temp->data;
    delete temp;
    length--;
    return tcb;
}

SleepingNode* Queue::popSTQ() {
    if (head == nullptr)
        return nullptr;
    Node* temp = head;
    head = head->next;
    SleepingNode* sleepingNode = (SleepingNode*)temp->data;
    delete temp;
    length--;
    return sleepingNode;
}

void* Queue::front() {
    if (head == nullptr)
        return nullptr;
    return head->data;
}

void* Queue::back() {
    if (tail == nullptr)
        return nullptr;
    return tail->data;
}

int Queue::size() {
    return length;
}

bool Queue::empty() {
    if (length == 0)
        return true;
    return false;
}

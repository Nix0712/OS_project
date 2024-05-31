#pragma once
#include "../lib/hw.h"

class TCB;
class SleepingNode {
  public:
    TCB* tcb;
    time_t wakeUpTime;
};
class Queue {
  public:
    Queue() : head(nullptr), tail(nullptr), length(0){};
    void pushTCB(TCB* data);
    void pushSortedSTQ(SleepingNode* data);
    TCB* popTCB();
    SleepingNode* popSTQ();
    void* front();
    void* back();
    int size();
    bool empty();
    void removeTCB(TCB* tcb);
    void removeSTQ(TCB* sn);

  private:
    struct Node {
        void* data;
        Node* next;
    };
    Node* head;
    Node* tail;
    int length;
};
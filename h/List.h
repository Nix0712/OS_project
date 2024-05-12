#pragma once

template <typename T>
class LinkedList {
  private:
    struct Node {
        T* data;
        Node* next;

        Node(T* data, Node* next) : data(data), next(next) {}
    };

    Node* head;
    Node* tail;

  public:
    LinkedList() : head(0), tail(0) {}

    LinkedList(const LinkedList<T>&) = delete;
    LinkedList<T>& operator=(const LinkedList<T>&) = delete;

    void insertFirst(T* data);
    void insertLast(T* data);

    void insertSorted(T* data);

    T* removeFirst();
    T* removeLast();

    T* peakFirst();
    T* peakLast();
};

template <typename T>
inline void LinkedList<T>::insertFirst(T* data) {
    Node* elem = new Node(data, head);
    head = elem;
    if (!tail)
        tail = head;
}

template <typename T>
inline void LinkedList<T>::insertLast(T* data) {
    Node* elem = new Node(data, 0);
    if (tail) {
        tail->next = elem;
        tail = elem;
    } else
        head = tail = elem;
}

template <typename T>
inline void LinkedList<T>::insertSorted(T* data) {
    if (head == 0)
        head = new Node(data, 0);
    Node* curr = head;
    Node* prev = 0;

    while (curr && curr->data->wakeUpTime < data->wakeUpTime) {
        prev = curr;
        curr = curr->next;
    }

    if (prev) {
        prev->next = new Node(data, curr);
    } else {
        head = new Node(data, curr);
    }
}

template <typename T>
inline T* LinkedList<T>::removeFirst() {
    if (!head)
        return 0;

    Node* tmp = head;
    head = head->next;

    if (!head)
        tail = 0;

    T* retData = tmp->data;
    delete tmp;

    return retData;
}

template <typename T>
inline T* LinkedList<T>::removeLast() {
    if (!head)
        return 0;

    Node* prev = 0;
    for (Node* curr = head; prev && prev != tail; curr = curr->next)
        prev = curr;

    Node* tmp = tail;
    if (prev)
        prev->next = 0;
    else
        head = 0;
    tail = prev;

    T* retData = tmp->data;
    delete tmp;
    return retData;
}

template <typename T>
inline T* LinkedList<T>::peakFirst() {
    if (!head)
        return 0;
    return head->data;
}

template <typename T>
inline T* LinkedList<T>::peakLast() {
    if (!tail)
        return 0;
    return tail->data;
}

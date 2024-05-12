#pragma once

#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/hw.h"
#include "../h/syscall_c.hpp"

class _Semaphore;

class TCB {
  public:
    using Body = void (*)(void*);

    TCB(Body body, void* arg, void* stack);

    uint64 getTimeSlice() const;

    bool isFinished() const;
    void setFinished(bool finished);

    bool isReady() const;
    void setReady(bool ready);

    int getWorkingSemaphore() const;
    void setWorkingSemaphore(int workingSemaphore);

    _Semaphore* getSemaphore() const;

    static void dispatch();

    static TCB* running;

    ~TCB() {
        if (stack != nullptr)
            delete stack;
    }

  private:
    // Context will have return adress and stack pointer rest of the registers will be saved on stack
    struct Context {
        uint64 ra;
        uint64 sp;
    };

    static void contextSwitch(Context* oldContext, Context* runningContext);

    Body body;
    void* arg;
    char* stack;
    Context context;
    uint64 timeSlice;
    bool finished;
    bool ready;
    int workingSemaphore; // -1 if it's not in semaphore and it's blocked, 0 if it's blocked and 1 if it's blocked
    _Semaphore* sem;      // If is connected to semaphore

    uint64 timeSliceCounter;

    friend class RiscV;

    static void threadWrapper();
};
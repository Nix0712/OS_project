#pragma once

#include "../h/hw.h"

class TCB {
  public:
    using Body = void (*)(void*);

    TCB(Body body, void* arg, void* stack);

    uint64 getTimeSlice() const;

    bool isFinished() const;
    void setFinished(bool finished);

    bool isReady() const;
    void setReady(bool ready);

    static void yield();

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

    static void dispatch();

    static void contextSwitch(Context* oldContext, Context* runningContext);

    Body body;
    void* arg;
    char* stack;
    Context context;
    uint64 timeSlice;
    bool finished;
    bool ready;

    uint64 timeSliceCounter;

    friend class RiscV;

    static void threadWrapper();
};
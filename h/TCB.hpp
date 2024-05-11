#pragma once

#include "../h/hw.h"

class TCB {
  public:
    using Body = void (*)();

    static TCB* createCoroutine(Body body);

    uint64 getTimeSlice() const;

    bool isFinished() const;
    void setFinished(bool finished);

    static void yield();

    static TCB* running;

    ~TCB() {
        if (stack != nullptr)
            delete[] stack;
    }

  private:
    TCB(Body body, uint64 timeSlice);
    // Context will have return adress and stack pointer rest of the registers will be saved on stack
    struct Context {
        uint64 ra;
        uint64 sp;
    };

    static void dispatch();

    // FINISH MANDLING IN ASSEMBLY and finish the sinq
    static void contextSwitch(Context* oldContext, Context* runningContext);

    Body body;
    uint64* stack;
    Context context;
    uint64 timeSlice;
    bool finished;

    static uint64 timeSliceCounter;

    friend class RiscV;

    static void threadWrapper();
};
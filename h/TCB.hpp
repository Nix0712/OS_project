#pragma once

#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/hw.h"
#include "../h/syscall_c.hpp"

class TCB {
  public:
    using Body = void (*)(void*);

    TCB(Body body, void* arg, void* stack);

    uint64 getTimeSlice() const;

    bool isFinished() const;
    void setFinished(bool finished);

    bool isReady() const;
    void setReady(bool ready);

    static void dispatch();

    static TCB* running;

    ~TCB() {
        if (stack != nullptr)
            mem_free(stack);
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

    uint64 timeSliceCounter;

    friend class RiscV;

    static void threadWrapper();
};
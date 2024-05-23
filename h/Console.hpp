#pragma once

#include "../h/Semaphore.hpp"
#include "../h/TCB.hpp"
#include "./hw.h"

class _Console {
  private:
    _Console();
    static char putcDataBuffer[1024];
    static char getcDataBuffer[1024];

    static int sendDataIndex;
    static int freeSendDataIndex;

    static int receiveDataIndex;
    static int freeReceiveDataIndex;

    static bool isConsoleInterrupted;

    static void getc_handler(); // Takes a character from controller to local buffer
    static void putc_handler(); // Takes a character from local buffer to controller

  public:
    void _putc(char chr);

    char _getc();

    void console_handler();

    static void getc_handler_wrapper(void*);
    static void putc_handler_wrapper(void*);

    static bool getIsConsoleInterrupted();

    static _Semaphore* mutex;

    _Console(_Console& other) = delete;
    void operator=(const _Console&) = delete;

    static _Console* GetInstance();
};
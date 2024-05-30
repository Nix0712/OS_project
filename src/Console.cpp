#include "../h/Console.hpp"
#include "../h/TCB.hpp"

char _Console::putcDataBuffer[1024];
char _Console::getcDataBuffer[1024];
int _Console::sendDataIndex = 0;
int _Console::freeSendDataIndex = 0;
int _Console::receiveDataIndex = 0;
int _Console::freeReceiveDataIndex = 0;
bool _Console::isConsoleInterrupted = false;
_Semaphore* _Console::mutex;

_Console::_Console() {
    mutex = new _Semaphore(0); // initialize mutex for getC
}

_Console* _Console::GetInstance() {
    static _Console consoleInstance;
    return &consoleInstance;
}

// This function is used to get data from the console to the buffer
void _Console::getc_handler() {
    _Console* console = _Console::GetInstance();
    while (true) {
        while (console->receiveDataIndex == (console->freeReceiveDataIndex + 1) % 1024)
            thread_dispatch();

        uint8 cstatus = *((char*)CONSOLE_STATUS);
        uint8 receiveBit = cstatus & CONSOLE_RX_STATUS_BIT;
        uint8 transferBit = cstatus & CONSOLE_TX_STATUS_BIT;
        if (receiveBit != 0 && console->isConsoleInterrupted) {
            console->getcDataBuffer[console->freeReceiveDataIndex] = *((char*)CONSOLE_RX_DATA);
            console->freeReceiveDataIndex = (console->freeReceiveDataIndex + 1) % 1024;
            mutex->signal(); // signal that data is ready to read
        } else {
            if (receiveBit == 0 && transferBit == 0 && console->isConsoleInterrupted) {
                console->isConsoleInterrupted = false;
                plic_complete(10);
            }
            thread_dispatch();
        }
    }
}

// This function is used to send data from the buffer to the console
void _Console::putc_handler() {
    _Console* console = _Console::GetInstance();
    while (true) {
        while (console->sendDataIndex == console->freeSendDataIndex)
            thread_dispatch();

        uint8 cstatus = *((char*)CONSOLE_STATUS);
        uint8 receiveBit = cstatus & CONSOLE_RX_STATUS_BIT;
        uint8 transferBit = cstatus & CONSOLE_TX_STATUS_BIT;
        if (transferBit != 0 && console->isConsoleInterrupted) {
            *((char*)CONSOLE_TX_DATA) = console->putcDataBuffer[console->sendDataIndex];
            console->sendDataIndex = (console->sendDataIndex + 1) % 1024;
        } else {
            if (receiveBit == 0 && transferBit == 0 && console->isConsoleInterrupted) {
                console->isConsoleInterrupted = false;
                plic_complete(10);
            }
            thread_dispatch();
        }
    }
}

// putC and getC used in sys calls

void _Console::_putc(char chr) {
    _Console* console = _Console::GetInstance();
    // In case that the buffer is full, wait until there is space to write
    while (sendDataIndex == (freeSendDataIndex + 1) % 1024)
        TCB::dispatch();
    console->putcDataBuffer[freeSendDataIndex] = chr;
    freeSendDataIndex = (freeSendDataIndex + 1) % 1024;
}

char _Console::_getc() {
    _Console* console = _Console::GetInstance();
    mutex->wait(); // Wait to receive data
    char c = console->getcDataBuffer[receiveDataIndex];
    receiveDataIndex = (receiveDataIndex + 1) % 1024;
    return c;
}

// It's just setting flag that we recived interrupt from console
void _Console::console_handler() {
    isConsoleInterrupted = true;
}

void _Console::getc_handler_wrapper(void*) {
    static bool isGetcInitialized = false;
    if (!isGetcInitialized) {
        isGetcInitialized = true;
        getc_handler();
    }
}

void _Console::putc_handler_wrapper(void*) {
    static bool isPutcInitialized = false;
    if (!isPutcInitialized) {
        isPutcInitialized = true;
        putc_handler();
    }
}

bool _Console::getIsConsoleInterrupted() {
    return isConsoleInterrupted;
}

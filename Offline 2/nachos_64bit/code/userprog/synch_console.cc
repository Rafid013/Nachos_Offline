//
// Created by rafid on 1/2/18.
//

#include <cstdio>
#include "synch_console.h"
#include "system.h"


static Semaphore* readAvailSemaphore = new Semaphore("Read Available Semaphore", 0);
static Semaphore* writeDoneSemaphore = new Semaphore("Write Done Semaphore", 0);

void SynchConsole::ReadAvail(void* arg) { readAvailSemaphore->V(); }
void SynchConsole::WriteDone(void* arg) { writeDoneSemaphore->V(); }

SynchConsole::SynchConsole() {
    lock = new Lock("Console Lock");
    console = new Console(NULL, NULL, ReadAvail, WriteDone, NULL);
}

SynchConsole::~SynchConsole() {
    delete readAvailSemaphore;
    delete writeDoneSemaphore;
    delete lock;
    delete console;
}


int SynchConsole::Read(int bufferAddr, int size) {
    int bytesRead = 0;
    char ch;
    readAvailSemaphore->P();
    lock->Acquire();
    while(bytesRead < size) {
        ch = console->GetChar();
        if(ch != EOF) {
            if(!machine->WriteMem(bufferAddr, 1, ch)) {
                lock->Release();
                return -1;
            }
            ++bytesRead;
            ++bufferAddr;
        }
        else break;
    }
    lock->Release();
    return bytesRead;
}


bool SynchConsole::Write(int bufferAddr, int size) {
    lock->Acquire();
    for(int i = 0; i < size; ++i) {
        int value;
        if(!machine->ReadMem(bufferAddr, 1, &value)) {
            lock->Release();
            return false;
        }
        console->PutChar((char)value);
        writeDoneSemaphore->P();
        ++bufferAddr;
    }
    lock->Release();
    return true;
}

bool SynchConsole::Write(char *buffer, int size) {
    lock->Acquire();
    for(int i = 0; i < size; ++i) {
        console->PutChar(buffer[i]);
        writeDoneSemaphore->P();
    }
    lock->Release();
    return true;
}



//
// Created by rafid on 1/2/18.
//

#ifndef SYNCH_CONSOLE_H
#define SYNCH_CONSOLE_H


#include "../machine/console.h"
#include "../threads/synch.h"

class SynchConsole {
    Console* console;
    //Semaphore *readAvailSemaphore;
    //Semaphore *writeDoneSemaphore;
    static void ReadAvail(void *arg);
    static void WriteDone(void *arg);
    Lock *lock;
public:
    SynchConsole();
    ~SynchConsole();
    int Read(int bufferAddr, int size);
    bool Write(int bufferAddr, int size);
    bool Write(char *buffer, int size);
};

//Semaphore* SynchConsole::readAvailSemaphore = new Semaphore("Read Available Semaphore", 0);
//Semaphore* SynchConsole::writeDoneSemaphore = new Semaphore("Write Done Semaphore", 0);


#endif //SYNCH_CONSOLE_H

// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include <cstdio>
#include "copyright.h"
#include "syscall.h"
#include "../machine/machine.h"
#include "../filesys/openfile.h"
#include "../userprog/addrspace.h"
#include "../threads/thread.h"
#include "../threads/system.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

bool readFileNameFromIntegerAddr(int addr, char *fileName) {
    int tempC;
    int i = 0;
    bool nullFound = false;
    while(!nullFound) {
        if(!machine->ReadMem(addr, 1, &tempC)) {
            return false;
        }
        ++addr;
        if(tempC != '\0') {
            fileName[i++] = (char)tempC;
        }
        else nullFound = true;
    }
    fileName[i] = '\0';
    return true;
}


SpaceId ExecCallInitialization(AddrSpace *space) {
    int fileNameAddr = machine->ReadRegister(4);
    char* fileName = new char[20];
    if(!readFileNameFromIntegerAddr(fileNameAddr, fileName)) {
        return 0;
    }
    OpenFile *executable = fileSystem->Open(fileName);
    if (executable == NULL) {
        printf("Unable to open file %s\n", fileName);
        return 0;
    }
    if(!space->Initialize(executable)) {
        return 0;
    }
    delete executable;
    SpaceId spaceId = spaceIdGenerator->generate();
    space->setSpaceId(spaceId);
    return spaceId;
}


void Execute(AddrSpace *space) {
    space->InitRegisters();
    space->RestoreState();
    machine->Run();
}


void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt)) {
        DEBUG('a', "Shutdown, initiated by user program.\n");
        interrupt->Halt();
    }
    else if((which == SyscallException) && (type == SC_Exec)){

        //exec initialization
        AddrSpace* space = new AddrSpace();
        SpaceId spaceId = ExecCallInitialization(space);


        //write result to register
        machine->WriteRegister(2, spaceId);
        if(spaceId != 0) {
            //start a new thread for this call
            Thread* thread = new Thread("Exec System Call");
            int table_index = processTable->add((void*)thread);
            thread->threadIndex = table_index;
            thread->space = space;
            thread->Fork((VoidFunctionPtr)&Execute, (void*)space);
        }


    }
    else if((which == SyscallException) && (type == SC_Exit)){
        int exitStatus = machine->ReadRegister(4);
        printf("Exit Status: %d\n", exitStatus);
        processTable->release(currentThread->threadIndex);
        SpaceId spaceId = currentThread->space->getSpaceId();
        spaceIdGenerator->releaseId(spaceId);
        delete currentThread->space;
        machine->WriteRegister(2, exitStatus);
        currentThread->Finish();
    }
    else {
        printf("Unexpected user mode exception %d %d\n", which, type);
        //ASSERT(false);
    }
    int pc = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, pc);
    pc = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, pc);
    pc += 4;
    machine->WriteRegister(NextPCReg, pc);
}

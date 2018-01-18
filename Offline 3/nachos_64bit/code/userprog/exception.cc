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

void pageFaultHandler() {
    int faultAddr = machine->ReadRegister(39);
    printf("Page Fault Exception at %d\n", faultAddr);
    int physPageNo;
    int virtualPageNo = faultAddr/PageSize;

    if(!currentThread->space->isSwapPageExists(virtualPageNo)) {
        printf("No Swap Page for this address\n");
        if (memoryManager->isAnyFreePage()) {

            physPageNo = memoryManager->Alloc(currentThread->threadIndex, &machine->pageTable[virtualPageNo]);
            printf("Allocating a free page\n");
        } else {
            printf("No free page available\n");
            physPageNo = memoryManager->AllocByForce();

            TranslationEntry *entry = memoryManager->pageTableEntry_For_ppn(physPageNo);


            int threadIndex = memoryManager->process_for_ppn(physPageNo);


            Thread *thread = (Thread*)processTable->get(threadIndex);

            int vpnToBeSwapped = entry->virtualPage;

            thread->space->saveIntoSwapSpace(vpnToBeSwapped);
            printf("Page moved to Swap Space from Main Memory\n");
            memoryManager->saveID_Entry_For_ppn(physPageNo, currentThread->threadIndex,
                                                &machine->pageTable[virtualPageNo]);
        }
        currentThread->space->loadIntoFreePage(faultAddr, physPageNo);
    }
    else {
        printf("Page is already in Swap Space\n");

        //get the physical page for the virtual page
        int ppn = machine->pageTable[virtualPageNo].physicalPage;

        if(memoryManager->pageIsAllocated(ppn)) {
            //get the all the info corresponding to the current owner of ppn
            TranslationEntry *entry = memoryManager->pageTableEntry_For_ppn(ppn);
            int threadIndex = memoryManager->process_for_ppn(ppn);

            int vpn_to_be_saved = entry->virtualPage;

            Thread *thread = (Thread *) processTable->get(threadIndex);

            //swap out the vpn for the current owner first
            thread->space->saveIntoSwapSpace(vpn_to_be_saved);
        }


        //save id and entry for the current thread for this ppn
        memoryManager->saveID_Entry_For_ppn(ppn, currentThread->threadIndex,
                                            &machine->pageTable[virtualPageNo]);

        //load from swap space
        currentThread->space->loadFromSwapSpace(virtualPageNo);

        printf("Page moved from Swap Space to Main Memory\n");
    }
    printf("\n\n");
}


bool readFileNameVirtualAddr(int addr, char *fileName) {
    int tempC;
    int i = 0;
    bool nullFound = false;
    while(!nullFound) {
        if(!machine->ReadMem(addr, 1, &tempC)) {
            pageFaultHandler();
            if(!machine->ReadMem(addr, 1, &tempC)) {
                return false;
            }
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
    if(!readFileNameVirtualAddr(fileNameAddr, fileName)) {
        return 0;
    }
    OpenFile *executable = fileSystem->Open(fileName);
    if (executable == NULL) {
        printf("Unable to open file %s\n", fileName);
        return 0;
    }
    space->Initialize(executable);
    //delete executable;
    SpaceId spaceId = spaceIdGenerator->generate();
    space->setSpaceId(spaceId);
    return spaceId;
}


void Execute(AddrSpace *space) {
    space->InitRegisters();
    space->RestoreState();
    machine->Run();
}

void ExecCall() {
    //exec initialization
    AddrSpace *space = new AddrSpace();
    SpaceId spaceId = ExecCallInitialization(space);

    //write result to register
    machine->WriteRegister(2, spaceId);
    if (spaceId != 0) {
        //start a new thread for this call
        Thread *thread = new Thread("Exec System Call");
        int table_index = processTable->add((void *) thread);
        thread->threadIndex = table_index;
        thread->space = space;
        thread->Fork((VoidFunctionPtr)&Execute, (void*)space);
    }
}

void ExitCall() {
    int exitStatus = machine->ReadRegister(4);
    printf("Exit Status: %d\n", exitStatus);
    processTable->release(currentThread->threadIndex);
    SpaceId spaceId = currentThread->space->getSpaceId();
    spaceIdGenerator->releaseId(spaceId);
    delete currentThread->space;
    machine->WriteRegister(2, exitStatus);
    currentThread->Finish();
}


void ReadCall() {
    //get parameters
    int bufferAddr = machine->ReadRegister(4);
    int size = machine->ReadRegister(5);
    int fileID = machine->ReadRegister(6);

    //check fileID
    if(fileID != ConsoleInput) {
        machine->WriteRegister(2, -1);
        return;
    }


    //read and store in buffer
    int bytesRead = synchConsole->Read(bufferAddr, size);
    machine->WriteRegister(2, bytesRead);
}


void WriteCall() {
    //get parameters
    int bufferAddr = machine->ReadRegister(4);
    int size = machine->ReadRegister(5);
    int fileID = machine->ReadRegister(6);

    //check fileID
    if(fileID != ConsoleOutput) {
        machine->WriteRegister(2, -1);
        return;
    }


    //write from buffer to console
    if(!synchConsole->Write(bufferAddr, size))
        machine->WriteRegister(2, -1);
    else machine->WriteRegister(2, 0);
}


void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt)) {
        DEBUG('a', "Shutdown, initiated by user program.\n");
        interrupt->Halt();
    }
    else if((which == SyscallException) && (type == SC_Exec)) {
        ExecCall();
    }
    else if((which == SyscallException) && (type == SC_Exit)) {
        ExitCall();
    }
    else if((which == SyscallException) && (type == SC_Read)) {
        ReadCall();
    }
    else if((which == SyscallException) && (type == SC_Write)) {
        WriteCall();
    }
    else if((which == PageFaultException)) {
        pageFaultHandler();
        return;
    }
    else {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(false);
    }
    int pc = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, pc);
    pc = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, pc);
    pc += 4;
    machine->WriteRegister(NextPCReg, pc);
}

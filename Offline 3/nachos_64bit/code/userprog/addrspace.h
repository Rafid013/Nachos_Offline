// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "../machine/translate.h"
#include "../filesys/openfile.h"
#include "../bin/noff.h"

#define UserStackSize		1024 	// increase this as necessary!

#define SWAP_SIZE 256

typedef int SpaceId;

class AddrSpace {
  public:
    AddrSpace();
    AddrSpace(OpenFile *executable);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch

    bool Initialize(OpenFile *executable);
    void setSpaceId(SpaceId spaceId);
    void releasePageTable();
    SpaceId getSpaceId();
    int loadIntoFreePage(int addr, int physicalPageNo);
    void loadSegment(int addr, int physicalPageNo);

    void saveIntoSwapSpace(int vpn);

    void loadFromSwapSpace(int vpn, int ppn);

    bool isSwapPageExists(int vpn);

  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    unsigned int numPages;		// Number of pages in the virtual 
					// address space
    SpaceId spaceId;
    OpenFile *executable;
    NoffHeader *noffH;

    char *swapSpace[SWAP_SIZE];
    bool swapOccupied[SWAP_SIZE];

};

#endif // ADDRSPACE_H

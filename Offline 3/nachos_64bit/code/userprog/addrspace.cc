// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include <cstring>
#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "../machine/machine.h"


//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}




//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace() {

}


bool AddrSpace::Initialize(OpenFile *executable) {

    //Executable Header related checks

    this->executable = executable;
    unsigned int i, size;

    noffH = new NoffHeader();

    executable->ReadAt((char *)noffH, sizeof(*noffH), 0);
    if ((noffH->noffMagic != NOFFMAGIC) &&
        (WordToHost(noffH->noffMagic) == NOFFMAGIC))
        SwapHeader(noffH);
    ASSERT(noffH->noffMagic == NOFFMAGIC);

    // how big is address space?
    size = noffH->code.size + noffH->initData.size + noffH->uninitData.size + UserStackSize;
    // we need to increase the size
    // to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    //check if it fits
    //if(!memoryManager->checkAndDecreasePageCount(numPages)) return false;

    if(numPages > NumPhysPages)
        return false;

    DEBUG('a', "Initializing address space, num pages %d, size %d\n",
          numPages, size);

    //creating page table
    pageTable = new TranslationEntry[numPages];
    for(i = 0; i < numPages; ++i) {
        pageTable[i].virtualPage = i;
        pageTable[i].physicalPage = -1;
        pageTable[i].valid = false;
        pageTable[i].use = false;
        pageTable[i].dirty = false;
        pageTable[i].readOnly = false;  // if the code segment was entirely on
        // a separate page, we could set its
        // pages to be read-only
    }

    //Zeroing Out The Address Space
    /*for(i = 0; i < numPages; ++i) {
        bzero(machine->mainMemory + pageTable[i].physicalPage * PageSize, PageSize);
    }*/


    //Loading Code and Data into the Address Space
    //return loadCodeSegment(noffH, executable, pageTable) && loadDataSegment(noffH, executable, pageTable);

}



void AddrSpace::releasePageTable() {
    for(int i = 0; i < numPages; ++i)
        if(pageTable[i].valid)
            memoryManager->FreePage(pageTable[i].physicalPage);
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Deallocate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    releasePageTable();
    delete pageTable;
    delete executable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------


void AddrSpace::setSpaceId(SpaceId spaceId) {
    this->spaceId = spaceId;
}

SpaceId AddrSpace::getSpaceId() {
    return spaceId;
}

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}


int AddrSpace::loadIntoFreePage(int addr, int physicalPageNo) {
    int virtualPageNo = addr/PageSize;
    pageTable[virtualPageNo].physicalPage = physicalPageNo;
    pageTable[virtualPageNo].valid = true;
    loadSegment(virtualPageNo, physicalPageNo);
    return 0;
}

void AddrSpace::loadSegment(int virtualPageNo, int physicalPageNo) {

    int inFileAddr, codeSize, dataSize;

    //the starting address for the virtual page
    int startingAddr = virtualPageNo*PageSize;

    //physical address for the physical page
    int physAddr = physicalPageNo*PageSize;

    //check if the starting address is in code segment
    if(startingAddr >= noffH->code.virtualAddr && startingAddr < noffH->code.virtualAddr + noffH->code.size) {

        //size of the code segment to be loaded
        codeSize = 0;
        if((noffH->code.size + noffH->code.virtualAddr - startingAddr) < PageSize)
            codeSize = noffH->code.size - startingAddr;
        else codeSize = PageSize;

        //in file address for startingAddr
        inFileAddr = noffH->code.inFileAddr + (startingAddr - noffH->code.virtualAddr);

        //load code segment starting from physical address
        executable->ReadAt(machine->mainMemory + physAddr, codeSize, inFileAddr);

        //if page is not fully loaded
        if(codeSize < PageSize) {
            //size of the data segment to be loaded
            dataSize = 0;
            if(noffH->initData.size < PageSize - codeSize) dataSize = noffH->initData.size;
            else dataSize = PageSize - codeSize;

            //in file address for data segment start
            inFileAddr = noffH->initData.inFileAddr;

            //load data segment starting from where code segment finished
            executable->ReadAt(machine->mainMemory + (physAddr + codeSize), dataSize, inFileAddr);
        }

        //if page is still not fully loaded
        if(codeSize + dataSize < PageSize) {
            //zero out the remaining page
            bzero(machine->mainMemory + (physAddr + codeSize + dataSize), PageSize - codeSize - dataSize);
        }
    }

    //check if the starting address is in data segment
    else if(startingAddr >= noffH->initData.virtualAddr
        && startingAddr < noffH->initData.virtualAddr + noffH->initData.size) {
        //size of the data segment to be loaded
        dataSize = 0;
        if(noffH->initData.size + noffH->initData.virtualAddr - startingAddr < PageSize)
            dataSize = noffH->initData.size + noffH->initData.virtualAddr - startingAddr;
        else dataSize = PageSize;

        //in file address for startingAddr
        inFileAddr = noffH->initData.inFileAddr + (startingAddr - noffH->initData.virtualAddr);

        //load data segment starting from physical address
        executable->ReadAt(machine->mainMemory + physAddr, dataSize, inFileAddr);

        //if page is not fully loaded
        if(dataSize < PageSize) {
            //zero out the remaining page
            bzero(machine->mainMemory + (physAddr + dataSize), PageSize - dataSize);
        }
    }

    //if in uninit segment and other cases
    else {
        bzero(machine->mainMemory + physAddr, PageSize);
    }
}
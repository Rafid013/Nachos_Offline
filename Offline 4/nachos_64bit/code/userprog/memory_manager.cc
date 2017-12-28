//
// Created by rafid on 12/28/17.
//

#include "memory_manager.h"


MemoryManager::MemoryManager(int numPages) {
    page_tracker = new BitMap(numPages);
    lock = new Lock("memory manager lock");
}

MemoryManager::~MemoryManager() {
    delete page_tracker;
    delete lock;
}

int MemoryManager::AllocPage() {
    lock->Acquire();
    int retValue = page_tracker->Find();
    lock->Release();
    return retValue;
}

void MemoryManager::FreePage(int physPageNum) {
    lock->Acquire();
    page_tracker->Clear(physPageNum);
    lock->Release();
}

bool MemoryManager::pageIsAllocated(int physPageNum) {
    lock->Acquire();
    bool retBool = page_tracker->Test(physPageNum);
    lock->Release();
    return retBool;
}
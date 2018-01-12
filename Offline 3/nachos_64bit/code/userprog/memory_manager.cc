//
// Created by rafid on 12/28/17.
//

#include "memory_manager.h"


MemoryManager::MemoryManager(int numPages) {
    page_tracker = new BitMap(numPages);
    lock = new Lock("memory manager lock");
    freePageCount = numPages;
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

bool MemoryManager::checkAndDecreasePageCount(int pageToAlloc_Count) {
    lock->Acquire();
    if(pageToAlloc_Count <= freePageCount) {
        freePageCount -= pageToAlloc_Count;
        lock->Release();
        return true;
    }
    lock->Release();
    return false;
}

bool MemoryManager::isAnyFreePage() {
    return (page_tracker->NumClear() > 0);
}
//
// Created by rafid on 12/28/17.
//

#include <cstdlib>
#include "memory_manager.h"


MemoryManager::MemoryManager(int numPages) {
    totalPage = numPages;
    page_tracker = new BitMap(numPages);
    lock = new Lock("memory manager lock");
    processMap = new int[numPages];
    entries = new TranslationEntry*[numPages];
    freePageCount = numPages;
}

MemoryManager::~MemoryManager() {
    delete page_tracker;
    delete lock;
    delete processMap;
    for(int i = 0; i < totalPage; ++i) {
        if(entries[i] != NULL) delete entries[i];
    }
    delete entries;
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


int MemoryManager::Alloc(int processNo, TranslationEntry *entry) {
    lock->Acquire();
    int retValue = page_tracker->Find();
    processMap[retValue] = processNo;
    entries[retValue] = entry;
    lock->Release();
    return retValue;
}


int MemoryManager::AllocByForce() {

    lock->Acquire();

    int lruPage = 0;

    for(int j = 1; j < totalPage; ++j) {
        if(entries[lruPage]->time_stamp > entries[j]->time_stamp) lruPage = j;
    }
    lock->Release();
    return lruPage;
}


int MemoryManager::process_for_ppn(int ppn)  {
    lock->Acquire();
    int ret = processMap[ppn];
    lock->Release();
    return ret;
}


TranslationEntry* MemoryManager::pageTableEntry_for_ppn(int ppn) {
    lock->Acquire();
    TranslationEntry *retEntry = entries[ppn];
    lock->Release();
    return retEntry;
}

void MemoryManager::saveID_Entry_For_ppn(int ppn, int id, TranslationEntry *entry) {
    lock->Acquire();
    processMap[ppn] = id;
    entries[ppn] = entry;
    lock->Release();
}
//
// Created by rafid on 12/28/17.
//

#ifndef CODE_MEMORY_MANAGER_H
#define CODE_MEMORY_MANAGER_H

#include "bitmap.h"
#include "../threads/synch.h"

class MemoryManager {
    BitMap *page_tracker;
    Lock *lock;
    int freePageCount;
public:
    explicit MemoryManager(int numPages);
    ~MemoryManager();
    int AllocPage();
    void FreePage(int physPageNum);
    bool pageIsAllocated(int physPageNum);
    bool checkAndDecreasePageCount(int pageToAlloc_Count);
    bool isAnyFreePage();
};
#endif //CODE_MEMORY_MANAGER_H

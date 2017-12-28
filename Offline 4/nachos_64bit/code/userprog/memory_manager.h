//
// Created by rafid on 12/28/17.
//

#ifndef CODE_MEMORY_MANAGER_H
#define CODE_MEMORY_MANAGER_H

#include "bitmap.h"
#include "synch.h"

class MemoryManager {
    BitMap *page_tracker;
    Lock *lock;
public:
    explicit MemoryManager(int numPages);
    ~MemoryManager();
    int AllocPage();
    void FreePage(int physPageNum);
    bool pageIsAllocated(int physPageNum);
};
#endif //CODE_MEMORY_MANAGER_H

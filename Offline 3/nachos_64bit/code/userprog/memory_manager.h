//
// Created by rafid on 12/28/17.
//

#ifndef CODE_MEMORY_MANAGER_H
#define CODE_MEMORY_MANAGER_H

#include "bitmap.h"
#include "../threads/synch.h"
#include "../machine/translate.h"

class MemoryManager {
    BitMap *page_tracker;
    Lock *lock;
    int freePageCount;
    int totalPage;
    int *processMap;
    TranslationEntry **entries;
public:
    explicit MemoryManager(int numPages);
    ~MemoryManager();
    int AllocPage();
    void FreePage(int physPageNum);
    bool pageIsAllocated(int physPageNum);
    bool checkAndDecreasePageCount(int pageToAlloc_Count);
    bool isAnyFreePage();
    int Alloc(int processNo, TranslationEntry *entry);
    int AllocByForce();
};
#endif //CODE_MEMORY_MANAGER_H

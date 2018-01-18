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

    //returns the process id allocated to physical page no
    int process_for_ppn(int ppn);

    TranslationEntry* pageTableEntry_For_ppn(int ppn);

    void saveID_Entry_For_ppn(int ppn, int id, TranslationEntry *entry);

};
#endif //CODE_MEMORY_MANAGER_H

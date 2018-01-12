//
// Created by rafid on 12/30/17.
//

#ifndef PROCESS_TABLE_H
#define PROCESS_TABLE_H

#include "../threads/synch.h"
#include <queue>
#include <cstdio>
using namespace std;

class Table {
    Lock *tableLock;
    void **table;
    queue<int> *Q;               //to keep track of unallocated slots
    int size;
public:
    explicit Table(int tableSize);
    ~Table();
    int add(void *item);            //returns the index of the item in table after adding
    void* get(int index);           //returns the object at index
    bool release(int index);    //frees the object at index
};
#endif //PROCESS_TABLE_H

//
// Created by rafid on 12/30/17.
//

#include "table.h"

Table::Table(int tableSize) {
    size = tableSize;
    table = new void*[size];
    tableLock = new Lock("Table Lock");
    Q = new queue<int>();
    for(int i = 0; i < size; ++i) {
        Q->push(i);
        table[i] = NULL;
    }
}

Table::~Table() {
    delete table;
    delete tableLock;
    delete Q;
}

int Table::add(void* item) {
    tableLock->Acquire();
    if(!Q->empty()) {
        int index = Q->front();
        Q->pop();
        table[index] = item;
        tableLock->Release();
        return index;
    }
    tableLock->Release();
    return -1;
}

void* Table::get(int index) {
    tableLock->Acquire();
    if(index > size || table[index] == NULL) {
        tableLock->Release();
        return NULL;
    }
    void* retValue = table[index];
    tableLock->Release();
    return retValue;
}

bool Table::release(int index) {
    tableLock->Acquire();
    if(index > size || table[index] == NULL) {
        tableLock->Release();
        return false;
    }
    table[index] = NULL;
    Q->push(index);
    tableLock->Release();
    return true;
}
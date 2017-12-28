#ifndef BUFFER_H
#define BUFFER_H

#include "synch.h"


class Buffer {
	int *array;
	int totalSize;
	int currentSize;
public:
	Lock *bufferLock;
	Condition *conditionFull;
	Condition *conditionEmpty;


	Buffer(int size, char* lockDebugName, char* condFullDebugName, char *condEmptyDebugName) {
		array = new int[totalSize = size];
		currentSize = 0;
		bufferLock = new Lock(lockDebugName);
		conditionFull = new Condition(condFullDebugName);
		conditionEmpty = new Condition(condEmptyDebugName);
	}


	~Buffer() {
		delete array;
	}

	int getTotalSize() { return totalSize; }
	int getCurrentSize() { return currentSize; }


	void insert(int item) {
		//bufferLock->Acquire();
		/*while(currentSize == totalSize) {
			conditionFull->Wait(bufferLock);
		}*/
		array[currentSize++] = item;
		//conditionEmpty->Broadcast();
		//bufferLock->Release();
	}

	
	int remove() {
		//bufferLock->Acquire();
		/*while(currentSize == 0) {
			conditionEmpty->Wait(bufferLock);
		}*/
		int retValue = array[--currentSize];
		//conditionFull->Broadcast();
		//bufferLock->Release();
		return retValue; 
	}
};

#endif



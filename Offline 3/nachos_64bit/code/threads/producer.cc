#include "producer.h"
#include <iostream>
using namespace std;

Producer::Producer(int idp, Buffer *buff) {
	this->id = idp;
	this->buffer = buff;
}

void Producer::produce() {
	while(1) {
		for(int i = 0; i < 100000; ++i)
			for(int j = 0; j < 1000; ++j);
		buffer->bufferLock->Acquire();
		while(buffer->getCurrentSize() == buffer->getTotalSize()) {
			buffer->conditionFull->Wait(buffer->bufferLock);
		}		
		int item = buffer->getCurrentSize();
		buffer->insert(item);
		if(buffer->getCurrentSize() == 1) buffer->conditionEmpty->Broadcast(buffer->bufferLock);
		cout << "Producer " << id << " produced item " << item << endl;
		buffer->bufferLock->Release(); 
	}
}



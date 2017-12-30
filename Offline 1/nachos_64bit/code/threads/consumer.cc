#include "consumer.h"
#include <iostream>
using namespace std;


Consumer::Consumer(int idc, Buffer *buff) {
	this->id = idc;
	this->buffer = buff;
}

void Consumer::consume() {
	while(1) {
		for(int i = 0; i < 100000; ++i)
			for(int j = 0; j < 1000; ++j);
		buffer->bufferLock->Acquire();
		while(buffer->getCurrentSize() == 0) {
			buffer->conditionEmpty->Wait(buffer->bufferLock);
		}
		int item = buffer->remove();
		if(buffer->getCurrentSize() == buffer->getTotalSize() - 1) buffer->conditionFull->Broadcast(buffer->bufferLock);
		cout << "Consumer " << id << " consumed item " << item << endl;
		buffer->bufferLock->Release();
	}
}

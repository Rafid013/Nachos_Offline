#include "buffer.h"
#include <iostream>

class Producer {
	int id;
	Buffer *buffer;
public:
	Producer(int idp, Buffer *buff);
	void produce();
};

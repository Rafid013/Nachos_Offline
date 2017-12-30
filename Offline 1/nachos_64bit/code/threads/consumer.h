#include "buffer.h"

class Consumer {
	int id;
	Buffer *buffer;
public:
	Consumer(int idc, Buffer *buff);
	void consume();
};

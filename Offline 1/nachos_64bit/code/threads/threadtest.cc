// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "consumer.h"
#include "producer.h"

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------


void produce(Producer *prod) {
	prod->produce();
}

void consume(Consumer *cons) {
	cons->consume();
}


void
ThreadTest()
{
    //DEBUG('t', "Entering SimpleTest");

    //Thread *t = new Thread("forked thread");

    //t->Fork(SimpleThread, 1);
    //SimpleThread(0);
	Buffer *buffer = new Buffer(20, "bufferlock", "full_condition", "empty_condition");
	Producer *prodArray[3];
	Consumer *consArray[3];
	for(int i = 0; i < 3; ++i) {
		prodArray[i] = new Producer(i + 1, buffer);
		consArray[i] = new Consumer(i + 1, buffer);
	}

	Thread *prodThreadArray[3];
	Thread *consThreadArray[3];
	
	prodThreadArray[0] = new Thread("Producer 1");
	prodThreadArray[1] = new Thread("Producer 2");
	prodThreadArray[2] = new Thread("Producer 3");

	consThreadArray[0] = new Thread("Consumer 1");
	consThreadArray[1] = new Thread("Consumer 2");
	consThreadArray[2] = new Thread("Consumer 3");

	for(int i = 0; i < 3; ++i) {
		prodThreadArray[i]->Fork((VoidFunctionPtr)produce, (void*)prodArray[i]);
		consThreadArray[i]->Fork((VoidFunctionPtr)consume, (void*)consArray[i]);
	}
}









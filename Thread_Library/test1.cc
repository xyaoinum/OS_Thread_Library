#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

int g = 0;
mutex mutex1;

void loop(void *a)
{
	char *id = (char *) a;
	int i;
	mutex1.lock();
	cout << "loop called with id " << id << endl;

	for (i=0; i<10; i++, g++) {
		cout << id << ":\t" << i << "\t" << g << endl;
		mutex1.unlock();
		if(i%3){
		thread::yield();
		}
		mutex1.lock();
	}
	cout << id << ":\t" << i << "\t" << g << endl;
	mutex1.unlock();
}

void parent(void *a)
{
	for(int i = 0; i < 100; i++){
		thread t1 ( (thread_startfunc_t) loop, (void *) "wtf");
	}
	intptr_t arg = (intptr_t) a;
	thread t1 ( (thread_startfunc_t) loop, (void *) "child thread1");
	thread::yield();
	thread::yield();
	thread::yield();
	t1.join();
	thread t2 ( (thread_startfunc_t) loop, (void *) "child thread2");
	thread t3 ( (thread_startfunc_t) loop, (void *) "child thread3");
	t1.join();
	thread t4 ( (thread_startfunc_t) loop, (void *) "child thread4");
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}

#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

int g = 0;

mutex mutex1;
cv cv1;

void loop(void *a)
{
	intptr_t id = (intptr_t) a;
	int i;

	cout << "loop called with id " << id << endl;

	mutex1.lock();
	for (i=0; i<5; i++, g++) {
		cout << id << ":\t" << i << "\t" << g << endl;
		mutex1.unlock();
		thread::yield();
		mutex1.lock();
	}
	cout << id << ":\t" << i << "\t" << g << endl;
	mutex1.unlock();
}

void parent(void *a)
{
	intptr_t arg = (intptr_t) a;

	cout << "parent called with arg " << arg << endl;
	thread t1 ( (thread_startfunc_t) loop, (void *) 1);
	thread t2 ( (thread_startfunc_t) loop, (void *) 2);
	thread t3 ( (thread_startfunc_t) loop, (void *) 3);
	thread t4 ( (thread_startfunc_t) loop, (void *) 4);
	thread t5 ( (thread_startfunc_t) loop, (void *) 5);
	thread t6 ( (thread_startfunc_t) loop, (void *) 6);
	thread t7 ( (thread_startfunc_t) loop, (void *) 7);
	thread t8 ( (thread_startfunc_t) loop, (void *) 8);
	thread t9 ( (thread_startfunc_t) loop, (void *) 9);
	thread t10 ( (thread_startfunc_t) loop, (void *) 10);
	mutex1.lock();
	thread::yield();    
	mutex1.unlock();
	cout<<"parent end"<<endl;
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}

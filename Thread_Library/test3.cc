#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

int g = 0;
mutex m1;
mutex m2;
mutex m3;

void loop(void *a)
{
	char *id = (char *) a;
	int i;

	cout << "loop called with id " << id << endl;
	m1.lock();
	m3.lock();
	for (i=0; i<5; i++, g++) {
		cout << id << ":\t" << i << "\t" << g << endl;
		m3.unlock();
		m2.lock();
		thread::yield();
		m3.lock();
		m2.unlock();
	}
	cout << id << ":\t" << i << "\t" << g << endl;
	m1.unlock();
	m3.unlock();
}

void parent(void *a)
{
	m1.lock();
//	m2.lock();
//	m3.lock();
	thread t1 ( (thread_startfunc_t) loop, (void *) "child thread1");
	thread t2 ( (thread_startfunc_t) loop, (void *) "child thread2");
	thread t3 ( (thread_startfunc_t) loop, (void *) "child thread3");
	thread t4 ( (thread_startfunc_t) loop, (void *) "child thread4");
	loop( (void *) "parent thread");
	m1.unlock();
//	m2.unlock();
//	m3.unlock();
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}

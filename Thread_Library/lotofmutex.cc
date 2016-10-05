#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

int g = 0;
mutex mutex1;
mutex mutex2;
mutex mutex3;
mutex mutex4;
mutex mutex5;
mutex mutex6;
mutex mutex7;
mutex mutex8;
mutex mutex9;
mutex mutex0;

void loop(void *a)
{
	char *id = (char *) a;
	int i;

	cout << "loop called with id " << id << endl;
	mutex1.lock();
	mutex2.lock();
	mutex3.lock();
	mutex4.lock();
	mutex5.lock();
	mutex6.lock();
	mutex7.lock();
	mutex8.lock();
	mutex9.lock();
	mutex0.lock();
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
	mutex2.unlock();
	mutex3.unlock();
	mutex4.unlock();
	mutex5.unlock();
	mutex6.unlock();
	mutex7.unlock();
	mutex8.unlock();
	mutex9.unlock();
	mutex0.unlock();
}

void parent(void *a)
{
	mutex1.lock();
	mutex2.lock();
	mutex3.lock();
	mutex4.lock();
	mutex5.lock();
	mutex6.lock();
	mutex7.lock();
	mutex8.lock();
	mutex9.lock();
	mutex0.lock();
	for(int i = 0; i < 100; i++){
		thread t1 ( (thread_startfunc_t) loop, (void *) "wtf");
	}

	intptr_t arg = (intptr_t) a;
	//mutex2.lock();
	cout << "parent called with arg " << arg << endl;
	thread t1 ( (thread_startfunc_t) loop, (void *) "child thread1");
	thread::yield();
	thread::yield();
	thread::yield();
	t1.join();
	thread t2 ( (thread_startfunc_t) loop, (void *) "child thread2");
	thread t3 ( (thread_startfunc_t) loop, (void *) "child thread3");
	t1.join();
	thread t4 ( (thread_startfunc_t) loop, (void *) "child thread4");
	loop( (void *) "parent thread");
		mutex1.unlock();
	mutex2.unlock();
	mutex3.unlock();
	mutex4.unlock();
	mutex5.unlock();
	mutex6.unlock();
	mutex7.unlock();
	mutex8.unlock();
	mutex9.unlock();
	mutex0.unlock();
}

int main()
{
	cpu::boot(8, (thread_startfunc_t) parent, (void *) 100, true, true, 0);
}

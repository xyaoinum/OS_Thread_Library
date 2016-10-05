#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

int g = 0;
mutex mutex1;
cv cv1;

void loop(void *a)
{
	cv1.signal();
	cout << "hehe" << endl;
}

void parent(void *a)
{
	cout << "hehe1" << endl;
	thread t1 ( (thread_startfunc_t) loop, (void *) "child thread1");
	mutex1.lock();
	cout << "hehe2" << endl;
	cv1.wait(mutex1);
	cout << "hehe3" << endl;
	mutex1.unlock();
	cout << "hehe4" << endl;
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}

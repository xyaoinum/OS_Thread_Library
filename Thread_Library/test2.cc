#include <iostream>
#include <cstdlib>
#include "thread.h"
#include <exception>
#include <stdexcept>

using namespace std;

int g = 0;
mutex mutex1;

thread *t;

void loop(void *a)
{
	t->join();
	cout << "wtf" << endl;

}

void parent(void *a)
{
	thread t1 ( (thread_startfunc_t) loop, (void *) "child thread1");
	t=&t1;
	cout << "gg" << endl;
	thread::yield();
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}

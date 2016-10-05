//self join
#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

thread *t;

void loop(void *a)
{
	cout << "hehe4" << endl;
	t->join();
	cout << "hehe" << endl;
}

void parent(void *a)
{
	cout << "hehe1" << endl;
	thread t1 ( (thread_startfunc_t) loop, (void *) "child thread1");
	t = &t1;
	cout << "hehe2" << endl;
	thread::yield();
	cout << "hehe3" << endl;
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}

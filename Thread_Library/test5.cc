//2 lock deadlock
#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

mutex m1;
mutex m2;

void loop2(void *a)
{
	cout << "wtf" << endl;
	m2.lock();
	cout << "wtf2" << endl;
	thread::yield();
	cout << "wtf3" << endl;
	m1.lock();
	cout << "wtf4" << endl;
	m2.unlock();
	cout << "wtf5" << endl;
	m1.unlock();
	cout << "wtf6" << endl;
}

void loop1(void *a)
{
	cout << "wtf" << endl;
	m1.lock();
	cout << "wtf2" << endl;
	thread::yield();
	cout << "wtf3" << endl;
	m2.lock();
	cout << "wtf4" << endl;
	m1.unlock();
	cout << "wtf5" << endl;
	m2.unlock();
	cout << "wtf6" << endl;
}

void parent(void *a)
{
	thread t0((thread_startfunc_t)loop1, (void *)0);
	thread t1((thread_startfunc_t)loop2, (void *)0);

}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}

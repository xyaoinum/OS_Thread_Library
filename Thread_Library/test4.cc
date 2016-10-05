//generate 100000 threads

#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

void requester(void *a)
{
	cout << "123" << endl;
}

void parent(void *a)
{
	cout << "2224" << endl;
	thread t0((thread_startfunc_t)requester, (void *)0);
	cout << "2225554" << endl;
	t0.join();
	cout << "2227774" << endl;
	t0.join();
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}

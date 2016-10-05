//cv queue
#include <iostream>
#include <cstdlib>
#include "thread.h"
#include <exception>
#include <stdexcept>

using namespace std;

void child0(void *a)
{
	cout << "child" << endl;
}

void parent(void *a)
{
	try{
		thread t1 ( (thread_startfunc_t) NULL, (void *) "child thread1");
	}catch(bad_alloc e){
		cout << "2122" << endl;
	}
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}

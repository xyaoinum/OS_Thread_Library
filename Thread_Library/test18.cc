#include <iostream>
#include <cstdlib>
#include "thread.h"
using namespace std;

thread* t;

void loop1(void *a)
{
	char * as = (char*)a;
	cout<<as<<" start"<<endl;
	t->join();
	cout<<as<<" end"<<endl;
}
void loop2(void *a)
{
	char * as = (char*)a;
	cout<<as<<" start"<<endl;
	thread::yield();
	cout<<as<<" end"<<endl;
}
void loop3(void *a)
{
	char * as = (char*)a;
	cout<<as<<" start"<<endl;
	t->join();
	cout<<as<<" end"<<endl;
}

void parent(void *a)
{
	intptr_t arg = (intptr_t) a;
	thread t1 ( (thread_startfunc_t)loop1, (void *)"loop1");
	thread t2 ( (thread_startfunc_t)loop2, (void *)"loop2");
	thread t3 ( (thread_startfunc_t)loop3, (void *)"loop3");
	t = &t2;
	t1.join();
	t3.join();
	t3.join();
	cout << "parent ends\n";
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}

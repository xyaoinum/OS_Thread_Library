#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;
int g =0;
int child = 0;
thread* t;

mutex mutex1;
mutex mutex2;
mutex mutex3;
cv cv1;
cv cv2;

void child3(void *a)
{
	mutex2.lock();	
	child++;
	cv1.signal();
	child++;
	cv1.signal();
	mutex2.unlock();
}

void child2(void *a)
{
	mutex2.lock();
	thread t4 ( (thread_startfunc_t) child3, (void *) "child thread3");
	t = &t4;
	cout<<"child2"<<endl;
	while(child > 1){
		cv1.wait(mutex2);
	}
	cout<<"child2"<<endl;
	while(child >= 2){
		cv1.wait(mutex2);
	}
	cout<<"child2"<<endl;
	mutex2.unlock();
}



void child1(void *a)
{
	char *id = (char *) a;
	int i;

	cout << "loop called with id " << id << endl;
	mutex1.lock();
	for (i=0; i<5; i++, g++) {
		cout << id << ":\t" << i << "\t" << g << endl;
		mutex1.unlock();
		thread::yield();
		mutex1.lock();
	}
	thread t3 ( (thread_startfunc_t) child2, (void *) "child thread2");
	cout << id << ":\t" << i << "\t" << g << endl;
	mutex1.unlock();
}

void parent(void *a)
{
	mutex1.lock();
	thread t1 ( (thread_startfunc_t) child1, (void *) "child thread1");
	//thread t2 ( (thread_startfunc_t) child1, (void *) "child thread1");
	mutex1.unlock();
	cout << "parent finish" << endl;
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}

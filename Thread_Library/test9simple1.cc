#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include "thread.h"

using namespace std;

thread* t0ptr;
thread* t1ptr;

void requester(void *a) {
	int reqN = (intptr_t)a;
	cout << reqN  <<" start" << endl;
	if (reqN == 0) {
		cout << reqN  <<" wait" << endl;
		t1ptr->join();
	}
	if (reqN == 1) {
		cout << reqN  <<" wait" << endl;
		t0ptr->join();
	}
	cout << reqN  <<" done" << endl;
}

void start(void *a) {
	thread t0((thread_startfunc_t)requester, (void *)0);
	thread t1((thread_startfunc_t)requester, (void *)1);
	t0ptr = &t0;
	t1ptr = &t1;
	thread::yield();
	
	cout << "finish" << endl;
}

int main(int argc, char *argv[]) {
	cpu::boot(1, (thread_startfunc_t) start, (void *) 100, false, false, 0);
}








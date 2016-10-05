#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include "thread.h"

using namespace std;

thread* t;

void requester(void *a) {
	int reqN = (intptr_t)a;
	if (reqN == 0) {
		t->join();
	}
	cout << reqN  <<" done" << endl;
}

void start(void *a) {
	thread t0((thread_startfunc_t)requester, (void *)0);
	thread t1((thread_startfunc_t)requester, (void *)1);
	t = &t1;
	t1.join();
	thread t2((thread_startfunc_t)requester, (void *)2);
}

int main(int argc, char *argv[]) {
	cpu::boot(1, (thread_startfunc_t) start, (void *) 100, false, false, 0);
}








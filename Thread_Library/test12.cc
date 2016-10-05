//unlock without lock

#include <iostream>
#include <cstdlib>
#include "thread.h"
#include <exception>
#include <stdexcept>

using namespace std;

mutex m1;
cv cv2;

void parent(void *a)

{
	cout << "334" << endl;
	m1.lock();
	cout << "334" << endl;
	m1.unlock();
	try{
	cout << "334" << endl;
	cv2.wait(m1);
	cout << "qqwe" << endl;
	}catch(runtime_error r){
	cout << "214242424" << endl;
	return;
	}
	cout << "gfgg" << endl;
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}

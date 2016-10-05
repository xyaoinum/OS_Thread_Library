#include <iostream>
#include <cstdlib>
#include "thread.h"
#include <exception>
#include <stdexcept>

using namespace std;

mutex m1;
mutex m2;
cv cv2;

void parent(void *a)

{
	m1.lock();
	m2.lock();
	cout << "gsadgdg" << endl;
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}

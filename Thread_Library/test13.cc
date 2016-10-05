#include <iostream>
#include <cstdlib>
#include "thread.h"
#include <exception>
#include <stdexcept>

using namespace std;

mutex m1;
cv cv2;

void loop(void *a)
{
try{
cout << "morning" << endl;
m1.unlock();
cout << "good"<< endl;
}catch(runtime_error){
cout << "fuccccc"<< endl;
return;
}
cout <<"ggg"<< endl;
}


void parent(void *a)
{
cout << "morning" << endl;
thread t1 ( (thread_startfunc_t) loop, (void *) "child thread1");
cout << "morning" << endl;
m1.lock();
cout << "morning" << endl;
thread::yield();
cout << "morning" << endl;
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}

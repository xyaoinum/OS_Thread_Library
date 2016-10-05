//Version 4.0
//Edited by HXZ on Feb.20

#include <iostream>
#include <ucontext.h>
#include <queue>
#include <unordered_map>
#include "thread.h"
#include <exception>
#include <stdexcept>

using namespace std;

/*************************************Structure created***************************************/

struct athread{
	ucontext_t * ucontext_ptr = NULL;
	queue<athread*> joinq;
};
struct alock{
	athread * owner = NULL;
	queue<athread *> waitq;
};
struct acv{
	queue<athread *> waitq;
};
struct thread::impl{
	athread *threading = NULL;
	bool complete = false;
};
struct cv::impl{
	acv condition;
};
struct mutex::impl{
	alock *locker = NULL;
};
struct cpu::impl{
	bool interrupt_allow = false;
	athread * current = NULL;
	ucontext_t * init_context = NULL;
};

/************************************Global Variables*****************************************/

queue<athread*> readyq;
queue<athread*> finishq;
queue<cpu*> cpu_waitq;
unordered_map<athread*, alock*> thread_lock_map;
unordered_map<athread*, thread::impl*> athread_thread_map;
bool initiate = false;


/****************************helper function to run a thread***********************************/
/***************enable interrupt and guard before func, disable them after func****************/
void func_helper(thread_startfunc_t func, void *arg){
	guard.store(false);
	cpu::self()->interrupt_enable();
	func(arg);
	cpu::self()->interrupt_disable();
	while(guard.exchange(true) == true){ }
	athread_thread_map[cpu::self()->impl_ptr->current]->complete = true;
	finishq.push(cpu::self()->impl_ptr->current);
	while(cpu::self()->impl_ptr->current->joinq.size() > 0){
		if(cpu_waitq.size() > 0){
			cpu_waitq.front()->impl_ptr->interrupt_allow = false;
			cpu_waitq.front()->impl_ptr->current = cpu::self()->impl_ptr->current->joinq.front();
			cpu_waitq.front()->interrupt_send();
			cpu_waitq.pop();
		}else{
			readyq.push(cpu::self()->impl_ptr->current->joinq.front());
		}
		cpu::self()->impl_ptr->current->joinq.pop();
	}
	swapcontext(cpu::self()->impl_ptr->current->ucontext_ptr, cpu::self()->impl_ptr->init_context);
}

/********************************Interrupt_Handler*********************************************/
/*********************Handler which take care of time interrupt********************************/

void timer_interrupt_handler(){	

	cpu::self()->interrupt_disable();
	while(guard.exchange(true) == true){}
	if(cpu::self()->impl_ptr->interrupt_allow == true && cpu::self()->impl_ptr->current != NULL){
		if(cpu_waitq.size() > 0){
			cpu_waitq.front()->impl_ptr->interrupt_allow = false;
			cpu_waitq.front()->impl_ptr->current = cpu::self()->impl_ptr->current;
			cpu_waitq.front()->interrupt_send();
			cpu_waitq.pop();
		}else{
			readyq.push(cpu::self()->impl_ptr->current);
		}
		swapcontext(cpu::self()->impl_ptr->current->ucontext_ptr, cpu::self()->impl_ptr->init_context);
	}
	guard.store(false);
	cpu::self()->interrupt_enable();
}

/*********************Handler which take care of IPI interrupt*********************************/

void IPI_interrupt_handler(){
	cpu::self()->interrupt_disable();
	while(guard.exchange(true) == true){ }
	cpu::self()->impl_ptr->interrupt_allow = true;
}

/********************************Implementation of cpu.h***************************************/

void cpu::init(thread_startfunc_t func, void *arg){


	while(guard.exchange(true) == true){ }
	try{
		cpu::self()->interrupt_vector_table[TIMER] = (interrupt_handler_t) timer_interrupt_handler;
		cpu::self()->interrupt_vector_table[IPI] = (interrupt_handler_t) IPI_interrupt_handler;
		initiate = true;
		impl_ptr = new impl;
		cpu::self()->impl_ptr->init_context = new ucontext_t;
	}catch(bad_alloc b){
		delete cpu::self()->impl_ptr->init_context;
		guard.store(false);
		cpu::self()->interrupt_enable();
		throw;
	}
	guard.store(false);
	if(func != NULL){
		cpu::self()->interrupt_enable();
		thread(func, arg);
		cpu::self()->interrupt_disable();
	}
	cpu::self()->impl_ptr->interrupt_allow = true;	
	while(guard.exchange(true) == true){ }
	getcontext(cpu::self()->impl_ptr->init_context);
	while(finishq.size() > 0){
		delete (char*) finishq.front()->ucontext_ptr->uc_stack.ss_sp;
		delete finishq.front()->ucontext_ptr;
		delete finishq.front();
		finishq.pop();
	}
	while(1){
		while(readyq.size() > 0){
			cpu::self()->impl_ptr->current = readyq.front();
			readyq.pop();
			swapcontext(cpu::self()->impl_ptr->init_context, cpu::self()->impl_ptr->current->ucontext_ptr);
			while(finishq.size() > 0){
				delete (char*) finishq.front()->ucontext_ptr->uc_stack.ss_sp;
				delete finishq.front()->ucontext_ptr;
				delete finishq.front();
				finishq.pop();
			}
			while(cpu_waitq.size() > 0 && readyq.size() > 0){
				cpu_waitq.front()->impl_ptr->interrupt_allow = false;
				cpu_waitq.front()->impl_ptr->current = readyq.front();
				readyq.pop();
				cpu_waitq.front()->interrupt_send();		
				cpu_waitq.pop();
				
			}
		}
		cpu::self()->impl_ptr->current = NULL;
		cpu_waitq.push(cpu::self());
		guard.store(false);
		cpu::interrupt_enable_suspend();
		swapcontext(cpu::self()->impl_ptr->init_context, cpu::self()->impl_ptr->current->ucontext_ptr);
		while(finishq.size() > 0){
			delete (char*) finishq.front()->ucontext_ptr->uc_stack.ss_sp;
			delete finishq.front()->ucontext_ptr;
			delete finishq.front();
			finishq.pop();
		}
	}

}

/********************************Implementation of thread.h**************************************/

thread::thread(thread_startfunc_t func, void *arg):impl_ptr(new impl){
	if(initiate == false || func == NULL){
		throw bad_alloc();
	}

	cpu::self()->interrupt_disable();
	while(guard.exchange(true) == true){ }
	try{
		/*Initialize a context structure*/
		impl_ptr->threading = new athread;
		impl_ptr->threading->ucontext_ptr = new ucontext_t;
		athread_thread_map[impl_ptr->threading] = impl_ptr;
		getcontext(impl_ptr->threading->ucontext_ptr);

		/*Direct the new thread to use a different stack.*/
		char *stack = new char [STACK_SIZE];
		impl_ptr->threading->ucontext_ptr->uc_stack.ss_sp = stack;
		impl_ptr->threading->ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
		impl_ptr->threading->ucontext_ptr->uc_stack.ss_flags = 0;
		impl_ptr->threading->ucontext_ptr->uc_link = nullptr;

		/*Direct the new thread to start by calling func*/
		makecontext(impl_ptr->threading->ucontext_ptr, (void (*)()) func_helper, 2, func, arg);
		if(cpu_waitq.size() > 0){
			cpu_waitq.front()->impl_ptr->interrupt_allow = false;
			cpu_waitq.front()->impl_ptr->current = impl_ptr->threading;
			cpu_waitq.front()->interrupt_send();
			cpu_waitq.pop();
		}else{
			readyq.push(impl_ptr->threading);
		}

	}catch(bad_alloc b){
		delete (char*) impl_ptr->threading->ucontext_ptr->uc_stack.ss_sp;
		delete impl_ptr->threading->ucontext_ptr;
		delete impl_ptr->threading;
		guard.store(false);
		cpu::self()->interrupt_enable();
		throw;
	}
	guard.store(false);
	cpu::self()->interrupt_enable();

}

thread::~thread(){ }

//thread::join is used to join a thread: the current thread is going to the back of joinq of 
//		the thread which call join, and it will only go into readyq after this thread is 
//		finished

void thread::join(){
	cpu::self()->interrupt_disable();
	while(guard.exchange(true) == true){ }
	if(impl_ptr->complete != true){                    
		impl_ptr->threading->joinq.push(cpu::self()->impl_ptr->current);
		swapcontext(cpu::self()->impl_ptr->current->ucontext_ptr, cpu::self()->impl_ptr->init_context);
	}
	guard.store(false);
	cpu::self()->interrupt_enable();
}

//thread::yield is used to yield: the thread which called yield will go to the back of ready 
//		queue and swap back to grab a new thread from the readyq

void thread::yield(){
	cpu::self()->interrupt_disable();  
	while(guard.exchange(true) == true){ }           
	if(cpu_waitq.size() > 0){
		cpu_waitq.front()->impl_ptr->interrupt_allow = false;
		cpu_waitq.front()->impl_ptr->current = cpu::self()->impl_ptr->current;
		cpu_waitq.front()->interrupt_send();
		cpu_waitq.pop();
	}else{
		readyq.push(cpu::self()->impl_ptr->current);
	}
	swapcontext(cpu::self()->impl_ptr->current->ucontext_ptr, cpu::self()->impl_ptr->init_context);
	guard.store(false);
	cpu::self()->interrupt_enable();
}

/********************************Implementation of mutex.h***************************************/

mutex::mutex():impl_ptr(new impl){
	try{
		impl_ptr->locker = new alock;
		impl_ptr->locker->owner = NULL;
	}catch(bad_alloc b){
		delete impl_ptr->locker;
		throw;
	}	
}

mutex::~mutex(){ 
	delete impl_ptr;
}

//mutex::lock is used for a thread to grab a lock, if the lock is free, the thread will grab the 
//	    lock immediately; if the lock is not free, the thread will goto this lock's waitq

void mutex::lock(){
	cpu::self()->interrupt_disable();
	while(guard.exchange(true) == true){ }
	if(impl_ptr->locker->owner == NULL){	
		impl_ptr->locker->owner = cpu::self()->impl_ptr->current;
	}else{
		impl_ptr->locker->waitq.push(cpu::self()->impl_ptr->current);
		swapcontext(cpu::self()->impl_ptr->current->ucontext_ptr, cpu::self()->impl_ptr->init_context);
	}
	guard.store(false);
	cpu::self()->interrupt_enable();	
}

//mutex::unlock is used for a thread to release a lock, if the lock does not belong to this thread 
//	    or it is a free lock, it will throw an exception 

void mutex::unlock(){
	cpu::self()->interrupt_disable();
	while(guard.exchange(true) == true){ }
	if(impl_ptr->locker->owner == NULL || impl_ptr->locker->owner != cpu::self()->impl_ptr->current){
		guard.store(false);
		cpu::self()->interrupt_enable();
		throw runtime_error("unlock() without lock held");
	}else{
		impl_ptr->locker->owner = NULL;
		if(impl_ptr->locker->waitq.size() > 0){
			impl_ptr->locker->owner = impl_ptr->locker->waitq.front();
			impl_ptr->locker->waitq.pop();
			if(cpu_waitq.size() > 0){
				cpu_waitq.front()->impl_ptr->interrupt_allow = false;
				cpu_waitq.front()->impl_ptr->current = impl_ptr->locker->owner;
				cpu_waitq.front()->interrupt_send();
				cpu_waitq.pop();
			}else{
				readyq.push(impl_ptr->locker->owner);
			}
		}
	}
	guard.store(false);
	cpu::self()->interrupt_enable();	
}

/********************************Implementation of cv.h******************************************/

cv::cv():impl_ptr(new impl){
	
}

cv::~cv(){
	delete impl_ptr;
}

//cv::wait is used for a condition variable to wait. During the wait, the current thread will release
//	 lock and when the wait function return it will grab the lock again. If the lock does not belong 
//    to this thread or it is a free lock, it will throw an exception 

void cv::wait(mutex& mutex){
	cpu::self()->interrupt_disable();	
	while(guard.exchange(true) == true){ }
	if(mutex.impl_ptr->locker->owner == NULL || mutex.impl_ptr->locker->owner != cpu::self()->impl_ptr->current){
		guard.store(false);
		cpu::self()->interrupt_enable();
		throw runtime_error("unlock() without lock held");
	}else{
		//thread_lock_map[cpu::self()->impl_ptr->current] = mutex.impl_ptr->locker;
		impl_ptr->condition.waitq.push(cpu::self()->impl_ptr->current);

		//unlock
		mutex.impl_ptr->locker->owner = NULL;
		if(mutex.impl_ptr->locker->waitq.size() > 0){
			mutex.impl_ptr->locker->owner = mutex.impl_ptr->locker->waitq.front();
			mutex.impl_ptr->locker->waitq.pop();
			if(cpu_waitq.size() > 0){
				cpu_waitq.front()->impl_ptr->interrupt_allow = false;
				cpu_waitq.front()->impl_ptr->current = mutex.impl_ptr->locker->owner;
				cpu_waitq.front()->interrupt_send();
				cpu_waitq.pop();
			}else{
				readyq.push(mutex.impl_ptr->locker->owner);
			}
		}
		swapcontext(cpu::self()->impl_ptr->current->ucontext_ptr, cpu::self()->impl_ptr->init_context);

		//reacquire the lock
		if(mutex.impl_ptr->locker->owner == NULL){	
			mutex.impl_ptr->locker->owner = cpu::self()->impl_ptr->current;
		}else{
			mutex.impl_ptr->locker->waitq.push(cpu::self()->impl_ptr->current);
			swapcontext(cpu::self()->impl_ptr->current->ucontext_ptr, cpu::self()->impl_ptr->init_context);
		}
	}
	guard.store(false);
	cpu::self()->interrupt_enable();	
}

//cv::signal is used for a thread to signal, after signal, if the waitq of the condition variable is 
//    not empty, the front of the waitq will go to readyq

void cv::signal(){
	cpu::self()->interrupt_disable();
	while(guard.exchange(true) == true){ }
	if(impl_ptr->condition.waitq.size() > 0){
		athread * temp_thread = impl_ptr->condition.waitq.front();
		impl_ptr->condition.waitq.pop();
		if(cpu_waitq.size() > 0){
			cpu_waitq.front()->impl_ptr->interrupt_allow = false;
			cpu_waitq.front()->impl_ptr->current = temp_thread;
			cpu_waitq.front()->interrupt_send();
			cpu_waitq.pop();
		}else{
			readyq.push(temp_thread);
		}

	}
	guard.store(false);
	cpu::self()->interrupt_enable();
}

//cv::broadcast is used for a thread to signal, after signal, if the waitq of the condition variable is 
//    not empty, all the threads in the waitq will go to readyq

void cv::broadcast(){
	cpu::self()->interrupt_disable();	
	while(guard.exchange(true) == true){ }
	while(impl_ptr->condition.waitq.size() > 0){
		athread * temp_thread = impl_ptr->condition.waitq.front();
		impl_ptr->condition.waitq.pop();
		if(cpu_waitq.size() > 0){
			cpu_waitq.front()->impl_ptr->interrupt_allow = false;
			cpu_waitq.front()->impl_ptr->current = temp_thread;
			cpu_waitq.front()->interrupt_send();
			cpu_waitq.pop();
		}else{
			readyq.push(temp_thread);
		}

	}
	guard.store(false);
	cpu::self()->interrupt_enable();
}





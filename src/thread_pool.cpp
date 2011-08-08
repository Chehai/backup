#include "thread_pool.h"

ThreadPool::ThreadPool(int total_size, int low_size)
: thread_pool_size(total_size), thread_pool_low_size(low_size)
{
	
}

int
ThreadPool::pushs(std::list<Task *>& ts)
{
	queue.pushs(ts);
	return 0;
}


int
ThreadPool::start()
{
	int i;
	for (i = 0; i < thread_pool_low_size; ++i) {
		thread_pool.create_thread(boost::bind(&ThreadPool::work, this, Task::Low));
	}
	for (; i < thread_pool_size; ++i) {
		thread_pool.create_thread(boost::bind(&ThreadPool::work, this, Task::High));
	}
	return 0;
}

void 
ThreadPool::work(Task::Priority pri)
{
	for (;;) {
		Task * t = queue.pop(pri);
		if (!t) {
			t->run();
		}
	}
}  	


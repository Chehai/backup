#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include "common.h"
#include "task_queue.h"
class ThreadPool {
public:
	ThreadPool(int, int);
	~ThreadPool();
	int start();
	int stop();
	int pushs(std::list<Task *>&);
private:
	int thread_pool_size;
	int thread_pool_low_size;
	TaskQueue queue;
	boost::thread_group thread_pool;
	void work(Task::Priority);  	
};
#endif
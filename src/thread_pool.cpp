#include "thread_pool.h"
#include "bye_task.h"
ThreadPool::ThreadPool(int total_size, int low_size)
: thread_pool_size(total_size), thread_pool_low_size(low_size)
{
	
}

ThreadPool::~ThreadPool()
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
		boost::thread * thr;
		thr = thread_pool.create_thread(boost::bind(&ThreadPool::work, this, Task::Low));
		if (!thr) {
			LOG(FATAL) << "ThreadPool::start: create thread failed";
		}
	}
	for (; i < thread_pool_size; ++i) {
		if (!thread_pool.create_thread(boost::bind(&ThreadPool::work, this, Task::High))) {
			LOG(FATAL) << "ThreadPool::start: create thread failed";
		}
	}
	return 0;
}

int
ThreadPool::stop()
{
	for (int i = 0; i < thread_pool_size; ++i) {
		ByeTask * t = new ByeTask();
		t->set_priority(Task::High);
		queue.push(t);
	}
	thread_pool.join_all();	
}

void 
ThreadPool::work(Task::Priority pri)
{
	for (;;) {
		Task * t = queue.pop(pri);
		if (t) {
			if (t->run() == -2) {
				LOG(INFO) << "ThreadPool::work: thread exits";
				delete t;
				return;
			}
		}
	}
}  	


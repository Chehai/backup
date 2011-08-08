#include "task_queue.h"
int 
TaskQueue::push(Task* t)
{
	boost::mutex::scoped_lock lk(queue_mutex);
	t->priority() == Task::High ? high_queue.push_back(t) : low_queue.push_back(t);
	queue_condition.notify_one();
	return 0;
}

int
TaskQueue::pushs(std::list<Task*>& tasks)
{
	boost::mutex::scoped_lock lk(queue_mutex);
	Task * t = tasks.front();
	for (std::list<Task *>::iterator iter = tasks.begin(); iter != tasks.end(); ++iter) {
		t->priority() == Task::High ? high_queue.push_back(*iter) : low_queue.push_back(*iter);
	}
	queue_condition.notify_all();
	return 0;
}

Task*
TaskQueue::pop(Task::Priority pri)
{
	Task * ret = NULL;
	boost::mutex::scoped_lock lk(queue_mutex);
	while (high_queue.empty() && low_queue.empty()) {
		queue_condition.wait(lk);
	}
	if (pri == Task::High) {
		if (!high_queue.empty()) {
			ret = high_queue.front();
			high_queue.pop_front();
		}
	} else {
		std::list<Task*>& queue = high_queue.empty() ? low_queue : high_queue;
		ret = queue.front();
		queue.pop_front();
	}
	if (!low_queue.empty() || !high_queue.empty()) {
		queue_condition.notify_one();
	}
	return ret;
}


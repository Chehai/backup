#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H
#include "common.h"
#include "task.h"
class TaskQueue {
public:
	int push(Task*);
	int pushs(std::list<Task*>&);
	Task* pop(Task::Priority);
private:
	std::list<Task*> low_queue;
	std::list<Task*> high_queue;
	boost::mutex queue_mutex;
	boost::condition queue_condition;
};
#endif
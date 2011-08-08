#ifndef PARENT_TASK_H
#define PARENT_TASK_H
#include "common.h"
#include "task.h"
class ParentTask : public Task {
public:
	int wait_children();
	int finish_child();
	int add_child(Task *);
	std::list<Task *>& children();
	ParentTask();
	virtual ~ParentTask();
private:
	std::list<Task *> children_tasks;
	int children_not_finish;
	boost::mutex children_mutex;
	boost::condition children_condition;
};
#endif
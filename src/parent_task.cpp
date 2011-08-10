#include "parent_task.h"

ParentTask::ParentTask()
: children_not_finish(0)
{	
}

ParentTask::~ParentTask()
{
}

int
ParentTask::wait_children()
{
	boost::mutex::scoped_lock lk(children_mutex);
	while (children_not_finish) {
		children_condition.wait(lk);
	}
	for(std::list<Task *>::iterator iter = children_tasks.begin(); iter != children_tasks.end(); ++iter) {
		delete *iter;
	}
	children_tasks.clear();
	return 0;
}

int
ParentTask::finish_child()
{
	boost::mutex::scoped_lock lk(children_mutex);
	--children_not_finish;
	children_condition.notify_one();
	return 0;
}

int
ParentTask::add_child(Task * child)
{
	children_tasks.push_back(child);
	++children_not_finish;
	return 0;
}

std::list<Task *>&
ParentTask::children()
{
	return children_tasks;
}

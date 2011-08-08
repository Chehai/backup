#include "task.h"

Task::Task()
{	
}

Task::~Task()
{
}

Task::Priority
Task::priority()
{
	return task_priority;
}

int
Task::set_priority(Task::Priority pri)
{
	task_priority = pri;
	return 0;
}
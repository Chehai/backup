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

Task::Status
Task::status() 
{
	return task_status;
}

int
Task::set_status(Task::Status st)
{
	task_status = st;
	return 0;
}

int
Task::run()
{
	return 0;
}
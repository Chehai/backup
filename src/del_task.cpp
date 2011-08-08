#include "del_task.h"

DelTask::DelTask(RemoteStore * rs, RemoteObject& ro, BackupTask& bt)
: remote_store(rs), remote_object(ro), parent_task(bt)
{
	parent_task.add_child(this);
}

int
DelTask::run()
{
	remote_store->del(remote_object);
	set_status(Task::Successful);
	parent_task.finish_child();
	return 0;
}
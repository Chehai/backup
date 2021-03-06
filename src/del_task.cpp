#include "del_task.h"

DelTask::DelTask(RemoteStore * rs, RemoteObject& ro, BackupTask& bt)
: remote_store(rs), remote_object(ro), parent_task(bt)
{
	parent_task.add_child(this);
	set_priority(Task::High);	
}

int
DelTask::run()
{
	if (remote_store->del(remote_object) < 0) {
		LOG(ERROR) << "DelTask::run: RemoteStore::del failed with " << remote_object.uri();
		set_status(Task::Failed);
	} else {
		set_status(Task::Successful);
	}
	parent_task.finish_child();
	return 0;
}
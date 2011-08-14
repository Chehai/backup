#include "put_task.h"

PutTask::PutTask(RemoteStore * rs, LocalObject& lo, BackupTask& bt)
: remote_store(rs), local_object(lo), parent_task(bt)
{
	parent_task.add_child(this);
	set_priority(Task::High);	
}

int
PutTask::run()
{
	if (remote_store->put(local_object) < 0) {
		LOG(ERROR) << "PutTask::run: RemoteStore::put failed with " << local_object.fs_path();
		set_status(Task::Failed);
	} else {
		set_status(Task::Successful);
	}
	parent_task.finish_child();
	return 0;
}
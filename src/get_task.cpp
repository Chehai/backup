#include "get_task.h"

GetTask::GetTask(RemoteStore * rs, RemoteObject& ro, boost::filesystem::path& dir, RestoreTask& t)
: remote_store(rs), remote_object(ro), restore_dir(dir), parent_task(t)
{
	parent_task.add_child(this);
	set_priority(Task::High);
}

int
GetTask::run()
{
	if (remote_store->get(remote_object, restore_dir) < 0) {
		LOG(ERROR) << "GetTask::run: RemoteStore::get failed with " << remote_object.uri();
		set_status(Task::Failed);
	} else {
		set_status(Task::Successful);
	}
	parent_task.finish_child();
	return 0;
}

#include "put_task.h"

PutTask::PutTask(RemoteStore * rs, LocalObject& lo, BackupTask& bt, bool gpg, std::string& rec)
: remote_store(rs), local_object(lo), parent_task(bt), use_gpg(gpg), gpg_recipient(rec)
{
	parent_task.add_child(this);
	set_priority(Task::High);	
}

int
PutTask::run()
{
	if (use_gpg) {
		// need a better way
		std::string cmd = "gpg --batch --yes -e -r '";
		std::string fs_path = local_object.fs_path().string();
		cmd += gpg_recipient;
		cmd += "' ";
		cmd += fs_path;
		if (system(cmd.c_str()) < 0) {
			LOG(ERROR) << "PutTask::run GnuPG failed";
			set_status(Task::Failed);
			parent_task.finish_child();
			return -1;
		} else {
			LOG(INFO) << "PutTask::run GnuPG done";
			local_object.set_fs_path(fs_path + ".gpg");
		}
	}
	
	if (remote_store->put(local_object) < 0) {
		LOG(ERROR) << "PutTask::run: RemoteStore::put failed with " << local_object.fs_path();
		set_status(Task::Failed);
		parent_task.finish_child();
		return -1;
	} else {
		set_status(Task::Successful);
		parent_task.finish_child();
		return 0;
	}
}
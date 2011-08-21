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
		std::string cmd = "gpg --batch --yes -r '";
		std::string fs_path = local_object.fs_path().string();
		cmd += gpg_recipient;
		cmd += "' -e ";
		cmd += fs_path;
		LOG(INFO) << cmd;
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
	
	int ret;
	if (remote_store->put(local_object) < 0) {
		LOG(ERROR) << "PutTask::run: RemoteStore::put failed with " << local_object.fs_path();
		set_status(Task::Failed);
		ret = -1;
	} else {
		set_status(Task::Successful);
		ret = 0;
	}
	if (use_gpg) {
		boost::system::error_code err;
		boost::filesystem::remove_all(local_object.fs_path(), err);
		if (err.value()) {
			LOG(ERROR) << "PutTask::run: remove_all failed " << err.message();
		}
	}
	parent_task.finish_child();
	return ret;
}
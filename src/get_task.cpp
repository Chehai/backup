#include "get_task.h"

GetTask::GetTask(RemoteStore * rs, RemoteObject& ro, boost::filesystem::path& dir, RestoreTask& t, bool gpg, std::string& rec)
: remote_store(rs), remote_object(ro), restore_dir(dir), parent_task(t), use_gpg(gpg), gpg_recipient(rec)
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
		parent_task.finish_child();
		return -1;
	} else {
		if (use_gpg) {
			boost::filesystem::path fs_path = restore_dir.parent_path();
			fs_path /= remote_object.uri();
			boost::filesystem::path gpg_fs_path = fs_path.string() + ".gpg";
			boost::system::error_code err;
			boost::filesystem::rename(fs_path, gpg_fs_path, err);
			if (err.value()) {
				LOG(ERROR) << "GetTask::run: rename failed " << err.message();
				set_status(Task::Failed);
				parent_task.finish_child();
				return -1;
			}
			std::string cmd = "gpg --batch --yes -r '";
			cmd += gpg_recipient;
			cmd += "' -o ";
			cmd += fs_path.string();
			cmd += " -d ";
			cmd += gpg_fs_path.string();
			if (system(cmd.c_str()) < 0) {
				LOG(ERROR) << "GetTask::run: gpg failed ";
				set_status(Task::Failed);
				parent_task.finish_child();
				return -1;
			}
			LOG(INFO) << "GetTask::run gpg done";
		}
		set_status(Task::Successful);
		parent_task.finish_child();
		return 0;
	}
}

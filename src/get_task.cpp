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
	boost::filesystem::path fs_path = restore_dir.parent_path();
	fs_path /= remote_object.uri();
	if (!use_gpg) {
		boost::system::error_code err;
		if (boost::filesystem::exists(fs_path, err) && !err.value()) {
			err.clear();
			std::size_t file_size = boost::filesystem::file_size(fs_path, err);
			if (!err.value() && file_size == remote_object.size()) {
				set_status(Task::Successful);
				parent_task.finish_child();
				LOG(INFO) << "GetTask::run finish run (already exists)" << remote_object.uri();
				return 0;
			}
		}
	}
	
	int ret;

	if (remote_store->get(remote_object, restore_dir) < 0) {
		LOG(ERROR) << "GetTask::run: RemoteStore::get failed with " << remote_object.uri();
		set_status(Task::Failed);
		ret = -1;
	} else {
		if (use_gpg) {
			boost::filesystem::path gpg_fs_path = fs_path.string() + ".gpg";
			boost::system::error_code err;
			boost::filesystem::rename(fs_path, gpg_fs_path, err);
			if (err.value()) {
				LOG(ERROR) << "GetTask::run: rename failed " << err.message();
				set_status(Task::Failed);
				ret = -1;
			} else {
				std::string cmd = "gpg --batch --yes -r '";
				cmd += gpg_recipient;
				cmd += "' -o ";
				cmd += fs_path.string();
				cmd += " -d ";
				cmd += gpg_fs_path.string();
				if (system(cmd.c_str()) < 0) {
					LOG(ERROR) << "GetTask::run: gpg failed ";
					set_status(Task::Failed);
					ret = -1;
				} else {
					LOG(INFO) << "GetTask::run gpg done";
					set_status(Task::Successful);
					ret = 0;
				}
				boost::system::error_code err;
				boost::filesystem::remove_all(gpg_fs_path, err);
				if (err.value()) {
					LOG(ERROR) << "GetTask::run: remove_all failed " << err.message();
				}
			}
		} else {
			set_status(Task::Successful);
			ret = 0;
		}
	}
	parent_task.finish_child();
	return ret;
}

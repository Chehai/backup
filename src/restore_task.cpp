#include "restore_task.h"

RestoreTask::RestoreTask(ThreadPool& tp, RemoteStore * rs, boost::filesystem::path& dir, std::string& prefix, std::time_t t, ParentTask& m)
: remote_store(rs), backup_prefix(prefix), objects_db_conn(NULL), thread_pool(tp), timestamp(t), parent_task(m)
{
	restore_dir = dir.filename().string() == "." ? dir.parent_path() : dir;
	boost::system::error_code err;
	boost::filesystem::path cur = boost::filesystem::current_path(err);
	if (err.value()) {
		objects_db_path = prefix + restore_dir.filename().string();
	} else {
		objects_db_path = cur;
		objects_db_path /= prefix + restore_dir.filename().string();
	}
	parent_task.add_child(this);
}

int
RestoreTask::open_database()
{
	if (!objects_db_conn) {
		if (sqlite3_open(objects_db_path.c_str(), &objects_db_conn) != SQLITE_OK) {
			close_database();
			return -1;		
		}
	}
	return 0;
}

int
RestoreTask::close_database()
{
	if (objects_db_conn) {
		sqlite3_close(objects_db_conn);
		objects_db_conn = NULL;
	}
	return 0;
}

bool
RestoreTask::dir_ok(const boost::filesystem::path& dir)
{
	boost::system::error_code err;
	return boost::filesystem::exists(dir, err) && !err.value() && boost::filesystem::is_directory(dir);
}

int
RestoreTask::run()
{
	if (dir_ok(restore_dir)) {
		open_database();
		RemoteObject::populate_remote_objects_table(objects_db_conn, remote_store, restore_dir, backup_prefix);		
		RemoteObject::find_to_get(objects_db_conn, timestamp, remote_objects_to_get);
		for (std::list<RemoteObject>::iterator iter = remote_objects_to_get.begin(); iter != remote_objects_to_get.end(); ++iter) {
			new GetTask(remote_store, *iter, restore_dir, *this);
		}
		thread_pool.pushs(children());
		wait_children();
		set_status(Task::Successful);
		close_database();
	}
	parent_task.finish_child();
	return 0;
}

#include "restore_task.h"

RestoreTask::RestoreTask(ThreadPool& tp, RemoteStore * rs, boost::filesystem::path& dir, std::string& prefix, std::time_t t, ParentTask& m)
: remote_store(rs), backup_prefix(prefix), objects_db_conn(NULL), thread_pool(tp), timestamp(t), parent_task(m)
{
	restore_dir = dir.filename().string() == "." ? dir.parent_path() : dir;
	boost::filesystem::create_directories(restore_dir);
	boost::system::error_code err;
	boost::filesystem::path cur = boost::filesystem::current_path(err);
	if (err.value()) {
		objects_db_path = prefix + restore_dir.filename().string() + ".db";
	} else {
		objects_db_path = cur;
		objects_db_path /= prefix + restore_dir.filename().string() + ".db";
	}
	parent_task.add_child(this);
	set_priority(Task::Low);
}

RestoreTask::~RestoreTask()
{
	close_database();
}

int
RestoreTask::open_database()
{
	if (!objects_db_conn) {
		if (sqlite3_open(objects_db_path.c_str(), &objects_db_conn) != SQLITE_OK) {
			LOG(FATAL) << "RestoreTask::open_database: " << sqlite3_errmsg(objects_db_conn);
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
		boost::filesystem::remove(objects_db_path);
	}
	return 0;
}

int
RestoreTask::run()
{
	LOG(INFO) << "RestoreTask::run: start restore " << restore_dir;		
	if (open_database() < 0) {
		set_status(Task::Failed);
		LOG(ERROR) << "RestoreTask::run: RestoreTask::open_database failed ";
		return -1;
	}
	if (RemoteObject::populate_remote_objects_table(objects_db_conn, remote_store, restore_dir, backup_prefix) < 0) {
		close_database();
		set_status(Task::Failed);
		LOG(ERROR) << "RestoreTask::run: RemoteObject::populate_remote_objects_table failed";
		return -1;
	}		
	if (RemoteObject::find_to_get(objects_db_conn, timestamp, remote_objects_to_get) < 0) {
		close_database();
		set_status(Task::Failed);
		LOG(ERROR) << "RestoreTask::run: RemoteObject::find_to_get failed";
		return -1;
	}
	for (std::list<RemoteObject>::iterator iter = remote_objects_to_get.begin(); iter != remote_objects_to_get.end(); ++iter) {
		GetTask * t = new GetTask(remote_store, *iter, restore_dir, *this);
		if (!t) {
			LOG(FATAL) << "RestoreTask::run: new GetTask failed";
		}
	}
	thread_pool.pushs(children());
	wait_children();
	set_status(Task::Successful);
	close_database();
	parent_task.finish_child();
	LOG(INFO) << "RestoreTask::run: finish restore " << restore_dir;
	return 0;
}

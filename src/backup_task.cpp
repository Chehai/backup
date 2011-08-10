#include "backup_task.h"

BackupTask::BackupTask(ThreadPool& tp, RemoteStore * rs, boost::filesystem::path& dir, std::string& prefix, ParentTask& m)
: remote_store(rs), backup_prefix(prefix), objects_db_conn(NULL), thread_pool(tp), parent_task(m)
{
	backup_dir = dir.filename().string() == "." ? dir.parent_path() : dir;
	boost::system::error_code err;
	boost::filesystem::path cur = boost::filesystem::current_path(err);
	if (err.value()) {
		objects_db_path = prefix + backup_dir.filename().string() + ".db";
	} else {
		objects_db_path = cur;
		objects_db_path /= prefix + backup_dir.filename().string() + ".db";
	}
	parent_task.add_child(this);
	set_priority(Task::Low);
}

int
BackupTask::open_database()
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
BackupTask::close_database()
{
	if (objects_db_conn) {
		sqlite3_close(objects_db_conn);
		objects_db_conn = NULL;
	}
	return 0;
}

bool
BackupTask::dir_ok(const boost::filesystem::path& dir)
{
	boost::system::error_code err;
	return boost::filesystem::exists(dir, err) && !err.value() && boost::filesystem::is_directory(dir);
}


int
BackupTask::run()
{		
	if (dir_ok(backup_dir)) {
		open_database();
		LocalObject::populate_local_objects_table(objects_db_conn, backup_dir, backup_prefix);
		RemoteObject::populate_remote_objects_table(objects_db_conn, remote_store, backup_dir, backup_prefix);
		LocalObject::find_to_put(objects_db_conn, local_objects_to_put);
		RemoteObject::find_to_del(objects_db_conn, remote_objects_to_del);
		for (std::list<LocalObject>::iterator iter = local_objects_to_put.begin(); iter != local_objects_to_put.end(); ++iter) {
			new PutTask(remote_store, *iter, *this);
		}
		for (std::list<RemoteObject>::iterator iter = remote_objects_to_del.begin(); iter != remote_objects_to_del.end(); ++iter) {
			new DelTask(remote_store, *iter, *this);
		}		
		thread_pool.pushs(children());
		wait_children();
		set_status(Task::Successful);
		close_database();
	}
	parent_task.finish_child();
	return 0;
}
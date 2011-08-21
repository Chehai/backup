#include "backup_task.h"

BackupTask::BackupTask(ThreadPool& tp, RemoteStore * rs, boost::filesystem::path& dir, std::string& prefix, bool gpg, std::string& rec, ParentTask& m)
: remote_store(rs), backup_prefix(prefix), objects_db_conn(NULL), thread_pool(tp), parent_task(m), use_gpg(gpg), gpg_recipient(rec)
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

BackupTask::~BackupTask()
{
	close_database();
}

int
BackupTask::open_database()
{
	if (!objects_db_conn) {
		if (sqlite3_open(objects_db_path.c_str(), &objects_db_conn) != SQLITE_OK) {
			LOG(ERROR) << "BackupTask::open_database: " << sqlite3_errmsg(objects_db_conn);
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
		boost::filesystem::remove(objects_db_path);
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
	LOG(INFO) << "BackupTask::run: start backup " << backup_dir;		
	if (dir_ok(backup_dir)) {
		if (open_database() < 0) {
			set_status(Task::Failed);
			LOG(ERROR) << "BackupTask::run: BackupTask::open_database failed ";
			return -1;
		}
		if (LocalObject::populate_local_objects_table(objects_db_conn, backup_dir, backup_prefix) < 0) {
			close_database();
			set_status(Task::Failed);
			LOG(ERROR) << "BackupTask::run: LocalObject::populate_local_objects_table failed";
			return -1;
		}
		if (RemoteObject::populate_remote_objects_table(objects_db_conn, remote_store, backup_dir, backup_prefix) < 0) {
			close_database();
			set_status(Task::Failed);
			LOG(ERROR) << "BackupTask::run: RemoteObject::populate_remote_objects_table failed";
			return -1;
		}
		if (LocalObject::find_to_put(objects_db_conn, local_objects_to_put) < 0) {
			close_database();
			set_status(Task::Failed);
			LOG(ERROR) << "BackupTask::run: LocalObject::find_to_put failed";
			return -1;
		}
		if (RemoteObject::find_to_del(objects_db_conn, remote_objects_to_del) < 0) {
			close_database();
			set_status(Task::Failed);
			LOG(ERROR) << "BackupTask::run: RemoteObject::find_to_del failed";
			return -1;
		}
		for (std::list<LocalObject>::iterator iter = local_objects_to_put.begin(); iter != local_objects_to_put.end(); ++iter) {
			PutTask * pt = new PutTask(remote_store, *iter, *this, use_gpg, gpg_recipient);
			if (!pt) {
				LOG(FATAL) << "BackupTask::run: new PutTask failed";
			}
		}
		for (std::list<RemoteObject>::iterator iter = remote_objects_to_del.begin(); iter != remote_objects_to_del.end(); ++iter) {
			DelTask * dt = new DelTask(remote_store, *iter, *this);
			if (!dt) {
				LOG(FATAL) << "BackupTask::run: new DelTask failed";
			}
		}		
		thread_pool.pushs(children());
		wait_children();
		set_status(Task::Successful);
		close_database();
	}
	parent_task.finish_child();
	LOG(INFO) << "BackupTask::run: finish backup " << backup_dir;			
	return 0;
}
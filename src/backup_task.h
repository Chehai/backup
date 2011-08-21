#ifndef BACKUP_TASK_H
#define BACKUP_TASK_H
#include "common.h"
#include "parent_task.h"
#include "remote_store.h"
#include "local_object.h"
#include "thread_pool.h"
#include "put_task.h"
#include "del_task.h"
class BackupTask : public ParentTask
{
public:
	BackupTask(ThreadPool& tp, RemoteStore * rs, boost::filesystem::path& dir, std::string& prefix, bool, std::string&, ParentTask& m);
	~BackupTask();
	int run();
private:
	ParentTask * backup;
	RemoteStore * remote_store;
	boost::filesystem::path objects_db_path;
	sqlite3 * objects_db_conn;
	boost::filesystem::path backup_dir;
	std::string backup_prefix;
	std::list<LocalObject> local_objects_to_put;
	std::list<RemoteObject> remote_objects_to_del;
	std::list<RemoteObject> remote_objects_to_get;
	ThreadPool& thread_pool;
	ParentTask& parent_task;
	bool use_gpg;
	std::string gpg_recipient;
	bool dir_ok(const boost::filesystem::path&);
	int open_database();
	int close_database();
};
#endif
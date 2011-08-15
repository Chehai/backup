#ifndef RESTORE_TASK_H
#define RESTORE_TASK_H
#include "common.h"
#include "parent_task.h"
#include "remote_store.h"
#include "local_object.h"
#include "thread_pool.h"
#include "get_task.h"
class RestoreTask : public ParentTask
{
public:
	RestoreTask(ThreadPool& tp, RemoteStore * rs, boost::filesystem::path& dir, std::string& prefix, std::time_t, ParentTask& m);
	~RestoreTask();
	int run();
private:
	RemoteStore * remote_store;
	boost::filesystem::path objects_db_path;
	sqlite3 * objects_db_conn;
	boost::filesystem::path restore_dir;
	std::string backup_prefix;
	std::list<RemoteObject> remote_objects_to_get;
	ThreadPool& thread_pool;
	std::time_t timestamp;
	ParentTask& parent_task;
	int open_database();
	int close_database();
};
#endif
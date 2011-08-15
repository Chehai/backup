#ifndef LIST_TASK_H
#define LIST_TASK_H
#include "common.h"
#include "task.h"
#include "parent_task.h"
#include "remote_store.h"
#include "remote_object.h"

class ListTask : public Task {
public:
	ListTask(RemoteStore * rs, boost::filesystem::path& dir, std::string& prefix, std::time_t&, ParentTask& m);
	int run();
private:
	int open_database();
	int close_database();
	std::string convert_to_string(std::time_t t);
	RemoteStore * remote_store;
	ParentTask& parent_task;
	boost::filesystem::path list_dir;
	std::string list_prefix;
	std::time_t timestamp;
	std::list<RemoteObject> remote_objects_to_list;
	sqlite3 * objects_db_conn;
	boost::filesystem::path objects_db_path;
	static boost::mutex list_mutex;
};
#endif
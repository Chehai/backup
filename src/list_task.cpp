#include "list_task.h"

boost::mutex ListTask::list_mutex;

ListTask::ListTask(RemoteStore * rs, boost::filesystem::path& dir, std::string& prefix, std::time_t& t, ParentTask& m)
: remote_store(rs), parent_task(m), list_prefix(prefix), timestamp(t), objects_db_conn(NULL)
{
	list_dir = dir.filename().string() == "." ? dir.parent_path() : dir;
	boost::system::error_code err;
	boost::filesystem::path cur = boost::filesystem::current_path(err);
	if (err.value()) {
		objects_db_path = prefix + list_dir.filename().string() + ".db";
	} else {
		objects_db_path = cur;
		objects_db_path /= prefix + list_dir.filename().string() + ".db";
	}
	parent_task.add_child(this);
	set_priority(Task::Low);
}

int
ListTask::open_database()
{
	if (!objects_db_conn) {
		if (sqlite3_open(objects_db_path.c_str(), &objects_db_conn) != SQLITE_OK) {
			LOG(FATAL) << "ListTask::open_database: " << sqlite3_errmsg(objects_db_conn);
			close_database();
			return -1;		
		}
	}
	return 0;
}

int
ListTask::close_database()
{
	if (objects_db_conn) {
		sqlite3_close(objects_db_conn);
		objects_db_conn = NULL;
		boost::filesystem::remove(objects_db_path);
	}
	return 0;
}

std::string
ListTask::convert_to_string(std::time_t t)
{
	return boost::posix_time::to_simple_string(boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(boost::posix_time::from_time_t(t)));
}

int
ListTask::run()
{
	LOG(INFO) << "ListTask::run: start list " << list_dir;		
	if (open_database() < 0) {
		set_status(Task::Failed);
		LOG(ERROR) << "ListTask::run: ListTask::open_database failed ";
		return -1;
	}
	if (RemoteObject::populate_remote_objects_table(objects_db_conn, remote_store, list_dir, list_prefix) < 0) {
		close_database();
		set_status(Task::Failed);
		LOG(ERROR) << "ListTask::run: RemoteObject::populate_remote_objects_table failed";
		return -1;
	}
	if (RemoteObject::find_to_get(objects_db_conn, timestamp, remote_objects_to_list) < 0) {
		close_database();
		set_status(Task::Failed);
		LOG(ERROR) << "ListTask::run: RemoteObject::find_to_get failed";
		return -1;
	}
	close_database();
	boost::mutex::scoped_lock lk(list_mutex);
	std::string timestamp_str = boost::posix_time::to_simple_string(boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(boost::posix_time::from_time_t(timestamp)));
	std::cout << "List " << list_prefix << list_dir.filename().string() << " at " << timestamp_str << " :" << std::endl;
	for (std::list<RemoteObject>::iterator iter = remote_objects_to_list.begin(); iter != remote_objects_to_list.end(); ++iter) {
		std::cout << iter->uri() << "\t" << convert_to_string(iter->updated_at()) << "\t" << iter->size() << "B" << std::endl;
	}
	set_status(Task::Successful);
	parent_task.finish_child();
	return 0;
}

#include <iostream>
#include "local_object.h"

LocalObject::LocalObject(const boost::filesystem::path& p, const std::string& uri)
: BackupObject(uri)
{
	local_fs_path = p;
	set_updated_at();
}

LocalObject::LocalObject(const boost::filesystem::path& p, const char * uri)
: BackupObject(uri)
{
	local_fs_path = p;
	set_updated_at();
};

LocalObject::LocalObject(const boost::filesystem::path& p)
: BackupObject(p.filename().string())
{
	local_fs_path = p;
	set_updated_at();
};


boost::filesystem::path&
LocalObject::fs_path()
{
	return local_fs_path;
}
int
LocalObject::set_updated_at()
{
	boost::system::error_code err;
	std::time_t t = boost::filesystem::last_write_time(local_fs_path, err);
	if (err.value()) {
		std::cout << "LocalObject::set_updated_at: " << err.message() << std::endl;
		set_status(BackupObject::Invalid);
		return -1;
	}
	object_updated_at = boost::posix_time::from_time_t(t);
	return 0;
}

int 
LocalObject::populate_local_objects_table(boost::filesystem::path& local_dir, boost::filesystem::path& backup_dir_prefix)
{
	boost::system::error_code err;
	
	if (boost::filesystem::exists(local_dir, err)) {
		if (err.value()) {
			return -1;
		}
		if (boost::filesystem::is_directory(local_dir)) {
			std::string sql = "CREATE TABLE IF NOT EXISTS local_objects(fs_path TEXT, updated_at INTEGER, uri TEXT)";
			if (sqlite3_exec(objects_db_conn, sql.c_str(), NULL, NULL, NULL) != SQLITE_OK) {
				return -1;
			}
			boost::filesystem::recursive_directory_iterator iter(local_dir), end_of_dir;
			for (; iter != end_of_dir; ++iter) {
				if (boost::filesystem::is_regular_file(iter->path())) {
					LocalObject lo(iter->path());
					// need reconsider uri method
					//remote_store->set_local_uri(local_object, lo);

				}
			}
		}
	}
}
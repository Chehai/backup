#include "local_object.h"

LocalObject::LocalObject(const boost::filesystem::path& file_path, const boost::filesystem::path& backup_dir, const std::string& backup_prefix)
{
	local_fs_path = file_path;
	
	set_status(BackupObject::Valid);
	
	boost::filesystem::path::iterator file_path_iter = file_path.begin();
	boost::filesystem::path::iterator backup_path_iter = back_dir.begin();
	boost::filesystem::path relative_path, uri_path;
	for(; file_path_iter != file_path.end() && backup_path_iter != back_dir.end(); ++file_path_iter, ++backup_path_iter);
	for (; file_path_iter != file_path.end(); ++file_path_iter) {
		relative_path /= *file_path_iter;
	}
	uri_path = backup_dir.filename();
	uri_path /= relative_path;
	set_uri(backup_prefix + uri_path.string());
	
	boost::system::error_code err;
	std::time_t t = boost::filesystem::last_write_time(local_fs_path, err);
	if (err.value()) {
		set_status(BackupObject::Invalid);
	}	
	set_updated_at(t);
}

int
LocalObject::insert_to_db()
{
	std::string sql = "INSERT INTO local_objects(fs_path, updated_at, uri) VALUES('";
	sql += local_fs_path.string();
	sql += "', ";
	sql += boost::lexical_cast<std::string>(updated_at);
	sql += ", '";
	sql += uri;
	sql += "')"
	if (sqlite3_exec(objects_db_conn, sql.c_str(), NULL, NULL, NULL) != SQLITE_OK) {
		return -1;
	}
	return 0;
}


boost::filesystem::path&
LocalObject::fs_path()
{
	return local_fs_path;
}

int 
LocalObject::populate_local_objects_table(const boost::filesystem::path& backup_dir, const std::string& backup_prefix)
{
	boost::system::error_code err;
	
	if (boost::filesystem::exists(backup_dir, err)) {
		if (err.value()) {
			return -1;
		}
		if (boost::filesystem::is_directory(backup_dir)) {
			std::string sql = "CREATE TABLE IF NOT EXISTS local_objects(fs_path TEXT, updated_at INTEGER, uri TEXT)";
			if (sqlite3_exec(objects_db_conn, sql.c_str(), NULL, NULL, NULL) != SQLITE_OK) {
				return -1;
			}
			boost::filesystem::recursive_directory_iterator iter(backup_dir), end_of_dir;
			for (; iter != end_of_dir; ++iter) {
				if (boost::filesystem::is_regular_file(iter->path())) {
					LocalObject lo(iter->path(), backup_dir, backup_prefix);
					lo.insert_to_db();
				}
			}
		}
	}
}
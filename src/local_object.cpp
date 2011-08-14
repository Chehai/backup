#include "local_object.h"

std::string LocalObject::find_by_uri_sql = "SELECT * FROM local_objects WHERE uri = ?";

LocalObject::LocalObject()
{	
}

LocalObject::LocalObject(const boost::filesystem::path& file_path, const boost::filesystem::path& backup_dir, const std::string& backup_prefix)
{
	local_fs_path = file_path;
	
	set_status(BackupObject::Valid);
	
	boost::filesystem::path real_backup_dir, relative_path, uri_path;
	real_backup_dir = backup_dir.filename().string() == "." ? backup_dir.parent_path() : backup_dir;
	boost::filesystem::path::iterator file_path_iter = file_path.begin();
	boost::filesystem::path::iterator backup_path_iter = real_backup_dir.begin();
	for(; file_path_iter != file_path.end() && backup_path_iter != real_backup_dir.end(); ++file_path_iter, ++backup_path_iter);
	for (; file_path_iter != file_path.end(); ++file_path_iter) {
		relative_path /= *file_path_iter;
	}
	uri_path = real_backup_dir.filename();
	uri_path /= relative_path;
	set_uri(backup_prefix + uri_path.string());
	
	boost::system::error_code err;
	std::time_t t = boost::filesystem::last_write_time(local_fs_path, err);
	if (err.value()) {
		set_status(BackupObject::Invalid);
	}	
	set_updated_at(t);
	
	err.clear();
	std::size_t file_size = boost::filesystem::file_size(local_fs_path, err);
	if (err.value()) {
		file_size = 0;
		set_status(BackupObject::Invalid);
	}
	set_size(file_size);
}

boost::filesystem::path&
LocalObject::fs_path()
{
	return local_fs_path;
}

int 
LocalObject::set_fs_path(const boost::filesystem::path& p)
{
	local_fs_path = p;
	return 0;
}

int 
LocalObject::populate_local_objects_table(sqlite3 * objects_db_conn, const boost::filesystem::path& backup_dir, const std::string& backup_prefix)
{
	std::string sql = "DROP TABLE IF EXISTS local_objects;CREATE TABLE IF NOT EXISTS local_objects(size INTEGER, fs_path TEXT, updated_at INTEGER, uri TEXT)";
	if (sqlite3_exec(objects_db_conn, sql.c_str(), NULL, NULL, NULL) != SQLITE_OK) {
		LOG(ERROR) << "LocalObject::populate_local_objects_table: sqlite3_exec " << sqlite3_errmsg(objects_db_conn);
		return -1;
	}
	sqlite3_stmt * stmt = NULL;
	sql = "INSERT INTO local_objects(fs_path, updated_at, uri, size) VALUES(?, ?, ?, ?)";
	if (sqlite3_prepare_v2(objects_db_conn, sql.c_str(), sql.length(), &stmt, NULL) != SQLITE_OK) {
	    LOG(ERROR) << "LocalObject::populate_local_objects_table: sqlite3_prepare_v2 " << sqlite3_errmsg(objects_db_conn);
		return -1;
	}
	boost::filesystem::recursive_directory_iterator iter(backup_dir), end_of_dir;
	for (; iter != end_of_dir; ++iter) {
		if (boost::filesystem::is_regular_file(iter->path())) {
			LocalObject lo(iter->path(), backup_dir, backup_prefix);
			if (sqlite3_bind_text(stmt, 1, lo.local_fs_path.c_str(), lo.local_fs_path.string().size(), SQLITE_STATIC) != SQLITE_OK) {
				LOG(ERROR) << "LocalObject::populate_local_objects_table: sqlite3_bind_text " << sqlite3_errmsg(objects_db_conn);
				break;
			}
			if (sqlite3_bind_int64(stmt, 2, (sqlite3_int64)lo.updated_at()) != SQLITE_OK) {
				LOG(ERROR) << "LocalObject::populate_local_objects_table: sqlite3_bind_int64 " << sqlite3_errmsg(objects_db_conn);
				break;
			}
			if (sqlite3_bind_text(stmt, 3, lo.uri().c_str(), lo.uri().size(), SQLITE_STATIC) != SQLITE_OK) {
				LOG(ERROR) << "LocalObject::populate_local_objects_table: sqlite3_bind_text " << sqlite3_errmsg(objects_db_conn);
				break;
			}
			if (sqlite3_bind_int64(stmt, 4, (sqlite3_int64)lo.size()) != SQLITE_OK) {
				LOG(ERROR) << "LocalObject::populate_local_objects_table: sqlite3_bind_int64 " << sqlite3_errmsg(objects_db_conn);
				break;
			}
			if (sqlite3_step(stmt) != SQLITE_DONE) {
				LOG(ERROR) << "LocalObject::populate_local_objects_table: sqlite3_step " << sqlite3_errmsg(objects_db_conn);
		    	break;
		  	}
			if (sqlite3_reset(stmt) != SQLITE_OK) {
				LOG(ERROR) << "LocalObject::populate_local_objects_table: sqlite3_reset " << sqlite3_errmsg(objects_db_conn);
				break;
			}			
		}
	}
	if (sqlite3_finalize(stmt) != SQLITE_OK) {
		LOG(ERROR) << "LocalObject::populate_local_objects_table: sqlite3_finalize " << sqlite3_errmsg(objects_db_conn);
		return -1;
	}
	return 0;
}

int
LocalObject::new_from_sqlite3(LocalObject& lo, int count, char ** results, char ** columns)
{
	for (int i = 0; i < count; ++i) {
		std::string column = columns[i];
		if (column == "fs_path") {
			lo.set_fs_path(boost::filesystem::path(results[i]));
		} else if (column == "updated_at") {
			lo.set_updated_at(boost::lexical_cast<std::time_t>(results[i]));
		} else if (column == "uri") {
			lo.set_uri(results[i]);
		} else if (column == "size") {
			lo.set_size(boost::lexical_cast<std::size_t>(results[i]));
		} else {
			LOG(ERROR) << "LocalObject::new_from_sqlite3 unknow column " << column; 
			return -1;
		}
	}
	lo.set_status(BackupObject::Valid);
	return 0;
}
int
LocalObject::sqlite3_find_by_callback(void * data , int count, char ** results, char ** columns)
{
	LocalObject * lo = (LocalObject *)data;
	new_from_sqlite3(*lo, count, results, columns);
	return 0;
}

LocalObject
LocalObject::find_by_uri(sqlite3 * objects_db_conn, const std::string& uri) // for test only
{
	std::string sql = "SELECT * FROM local_objects WHERE uri = '";
	sql += uri;
	sql += "' LIMIT 1";
	LocalObject lo;
	lo.set_status(BackupObject::Invalid);
	if (sqlite3_exec(objects_db_conn, sql.c_str(), sqlite3_find_by_callback, &lo, NULL) != SQLITE_OK) {
		;
	}
	return lo;
}

int
LocalObject::sqlite3_find_to_put_callback(void * data , int count, char ** results, char ** columns)
{
	std::list<LocalObject> * objects_to_put = (std::list<LocalObject> *)data;
	LocalObject lo;
	lo.set_status(BackupObject::Invalid);
	new_from_sqlite3(lo, count, results, columns);
	if (lo.status() == BackupObject::Valid) {
		objects_to_put->push_back(lo);
	}
	return 0;
}

int
LocalObject::find_to_put(sqlite3 * objects_db_conn, std::list<LocalObject>& res)
{
	std::string sql = "SELECT lo.* FROM local_objects lo LEFT JOIN remote_objects ro ON lo.uri = ro.uri AND lo.updated_at <= ro.updated_at WHERE ro.uri IS NULL";
	if (sqlite3_exec(objects_db_conn, sql.c_str(), sqlite3_find_to_put_callback, &res, NULL) != SQLITE_OK) {
		LOG(ERROR) << "LocalObject::find_to_put: sqlite3_exec " << sqlite3_errmsg(objects_db_conn);
		return -1;
	}
	return 0;
}
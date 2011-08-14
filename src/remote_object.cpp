#include "remote_object.h"

RemoteObject::RemoteObject(const std::string& uri, const std::time_t& t, char act)
:BackupObject(uri, t)
{
	set_action(act);
}

RemoteObject::RemoteObject()
{
	set_action(char(0));
}

char
RemoteObject::action() const
{
	return object_action;
}

int
RemoteObject::set_action(char act)
{
	object_action = act;
	return 0;
}

int 
RemoteObject::populate_remote_objects_table(sqlite3 * objects_db_conn, RemoteStore * remote_store, const boost::filesystem::path& backup_dir, const std::string& backup_prefix)
{

	std::string prefix = backup_prefix + backup_dir.filename().string();
	std::list<RemoteObject> remote_objects;
	if (remote_store->list(prefix, remote_objects) < 0) {
		LOG(ERROR) << "RemoteObject::populate_remote_objects_table: remote_store::list failed with " << prefix;
		return -1;
	}
	std::string sql = "DROP TABLE IF EXISTS remote_objects;CREATE TABLE IF NOT EXISTS remote_objects(updated_at INTEGER, uri TEXT, action TEXT)";
	if (sqlite3_exec(objects_db_conn, sql.c_str(), NULL, NULL, NULL) != SQLITE_OK) {
		LOG(ERROR) << "RemoteObject::populate_remote_objects_table: sqlite3_exec " << sqlite3_errmsg(objects_db_conn);
		return -1;
	}
	sqlite3_stmt * stmt = NULL;
	sql = "INSERT INTO remote_objects(action, updated_at, uri) VALUES(?, ?, ?)";
	if (sqlite3_prepare_v2(objects_db_conn, sql.c_str(), sql.length(), &stmt, NULL) != SQLITE_OK) {
	    LOG(ERROR) << "RemoteObject::populate_remote_objects_table: sqlite3_prepare_v2 " << sqlite3_errmsg(objects_db_conn);
		return -1;
	}

	for (std::list<RemoteObject>::iterator iter = remote_objects.begin() ; iter != remote_objects.end(); ++iter) {
		if (sqlite3_bind_text(stmt, 1, &iter->object_action, 1, SQLITE_STATIC) != SQLITE_OK) {
			LOG(ERROR) << "RemoteObject::populate_remote_objects_table: sqlite3_bind_text " << sqlite3_errmsg(objects_db_conn);
			break;
		}
		if (sqlite3_bind_int64(stmt, 2, (sqlite3_int64)iter->updated_at()) != SQLITE_OK) {
			LOG(ERROR) << "RemoteObject::populate_remote_objects_table: sqlite3_bind_int64 " << sqlite3_errmsg(objects_db_conn);
			break;
		}
		if (sqlite3_bind_text(stmt, 3, iter->uri().c_str(), iter->uri().size(), SQLITE_STATIC) != SQLITE_OK) {
			LOG(ERROR) << "RemoteObject::populate_remote_objects_table: sqlite3_bind_text " << sqlite3_errmsg(objects_db_conn);
			break;
		}
		if (sqlite3_step(stmt) != SQLITE_DONE) {
	    	LOG(ERROR) << "RemoteObject::populate_remote_objects_table: sqlite3_step " << sqlite3_errmsg(objects_db_conn);
			break;
	  	}
		if (sqlite3_reset(stmt) != SQLITE_OK) {
			LOG(ERROR) << "RemoteObject::populate_remote_objects_table: sqlite3_reset " << sqlite3_errmsg(objects_db_conn);
			break;
		}
	}
	if (sqlite3_finalize(stmt) != SQLITE_OK) {
		LOG(ERROR) << "RemoteObject::populate_remote_objects_table: sqlite3_finalize " << sqlite3_errmsg(objects_db_conn);
		return -1;
	}
	return 0;
}

int
RemoteObject::new_from_sqlite3(RemoteObject& ro, int count, char ** results, char ** columns)
{
	for (int i = 0; i < count; ++i) {
		std::string column = columns[i];
		if (column == "updated_at") {
			ro.set_updated_at(boost::lexical_cast<std::time_t>(results[i]));
		} else if (column == "uri") {
			ro.set_uri(results[i]);
		} else if (column == "action") {
			ro.set_action(results[i][0]);
		} else {
			LOG(ERROR) << "RemoteObject::new_from_sqlite3 unknow column " << column; 
			return -1;
		}
	}
	ro.set_status(BackupObject::Valid);
	return 0;
}


int
RemoteObject::sqlite3_find_by_callback(void * data , int count, char ** results, char ** columns)
{
	RemoteObject * ro = (RemoteObject *)data;
	new_from_sqlite3(*ro, count, results, columns);
	return 0;
}

RemoteObject
RemoteObject::find_by_uri(sqlite3 * objects_db_conn, const std::string& uri) // for test only
{
	std::string sql = "SELECT * FROM remote_objects WHERE uri = '";
	sql += uri;
	sql += "' LIMIT 1";
	RemoteObject ro;
	ro.set_status(BackupObject::Invalid);
	if (sqlite3_exec(objects_db_conn, sql.c_str(), sqlite3_find_by_callback, &ro, NULL) != SQLITE_OK) {
		;
	}
	return ro;
}


int
RemoteObject::sqlite3_find_to_del_callback(void * data , int count, char ** results, char ** columns)
{
	std::list<RemoteObject> * objects_to_delete = (std::list<RemoteObject> *)data;
	RemoteObject ro;
	ro.set_status(BackupObject::Invalid);
	new_from_sqlite3(ro, count, results, columns);
	if (ro.status() == BackupObject::Valid) {
		objects_to_delete->push_back(ro);
	}
	return 0;
}

int
RemoteObject::find_to_del(sqlite3 *	objects_db_conn, std::list<RemoteObject>& res)
{
	std::string sql = "SELECT ro.* FROM remote_objects ro JOIN (SELECT ro1.uri, MAX(ro1.updated_at) FROM remote_objects ro1 GROUP BY ro1.uri) t ON t.uri = ro.uri LEFT JOIN local_objects lo ON ro.uri = lo.uri WHERE lo.uri IS NULL AND ro.action = 'u'";
	if (sqlite3_exec(objects_db_conn, sql.c_str(), sqlite3_find_to_del_callback, &res, NULL) != SQLITE_OK) {
		LOG(ERROR) << "RemoteObject::find_to_del: sqlite3_exec " << sqlite3_errmsg(objects_db_conn);
		return -1;
	}
	return 0;
}

int
RemoteObject::sqlite3_find_to_get_callback(void * data , int count, char ** results, char ** columns)
{
	std::list<RemoteObject> * objects_to_get = (std::list<RemoteObject> *)data;
	RemoteObject ro;
	ro.set_status(BackupObject::Invalid);
	new_from_sqlite3(ro, count, results, columns);
	if (ro.status() == BackupObject::Valid) {
		objects_to_get->push_back(ro);
	}
	return 0;
}


int
RemoteObject::find_to_get(sqlite3 * objects_db_conn, const std::time_t timestamp, std::list<RemoteObject>& res)
{
	std::string sql = "SELECT ro1.* FROM remote_objects ro1 JOIN (SELECT ro.uri uri, MAX(ro.updated_at) max_updated_at FROM remote_objects ro where ro.updated_at <= ";
	sql += boost::lexical_cast<std::string>(timestamp);
	sql += " GROUP BY ro.uri) t ON ro1.uri = t.uri AND ro1.updated_at = t.max_updated_at WHERE ro1.action = 'u' ORDER BY ro1.uri";
	if (sqlite3_exec(objects_db_conn, sql.c_str(), sqlite3_find_to_get_callback, &res, NULL) != SQLITE_OK) {
		LOG(ERROR) << "RemoteObject::find_to_get: sqlite3_exec " << sqlite3_errmsg(objects_db_conn);
		return -1;
	}
	return 0;
}

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
RemoteObject::action()
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
RemoteObject::insert_to_db()
{
	std::string sql = "INSERT INTO remote_objects(updated_at, uri, action) VALUES(";
	sql += boost::lexical_cast<std::string>(updated_at());
	sql += ", '";
	sql += uri();
	sql += "', '";
	sql += object_action;
	sql += "')";
	if (sqlite3_exec(objects_db_conn, sql.c_str(), NULL, NULL, NULL) != SQLITE_OK) {
		return -1;
	}
	return 0;
}

int 
RemoteObject::populate_remote_objects_table(RemoteStore * remote_store, const boost::filesystem::path& backup_dir, const std::string& backup_prefix)
{
	boost::system::error_code err;
	
	if (boost::filesystem::exists(backup_dir, err)) {
		if (err.value()) {
			return -1;
		}
		if (boost::filesystem::is_directory(backup_dir)) {
			std::string sql = "DROP TABLE IF EXISTS remote_objects";
			if (sqlite3_exec(objects_db_conn, sql.c_str(), NULL, NULL, NULL) != SQLITE_OK) {
				return -1;
			}
			sql = "CREATE TABLE IF NOT EXISTS remote_objects(updated_at INTEGER, uri TEXT, action TEXT)";
			if (sqlite3_exec(objects_db_conn, sql.c_str(), NULL, NULL, NULL) != SQLITE_OK) {
				return -1;
			}
			std::string prefix = backup_prefix + backup_dir.filename().string();
			std::list<RemoteObject> remote_objects;
			remote_store->list(prefix, remote_objects);
			std::list<RemoteObject>::iterator iter;
			for (iter = remote_objects.begin() ; iter != remote_objects.end(); ++iter) {
				iter->insert_to_db();
			}
		}
	}
}

int
RemoteObject::sqlite3_find_by_callback(void * data , int count, char ** results, char ** columns)
{
	RemoteObject * ro = (RemoteObject *)data;
	for (int i = 0; i < count; ++i) {
		std::string column = columns[i];
		if (column == "updated_at") {
			ro->set_updated_at(boost::lexical_cast<std::time_t>(results[i]));
		} else if (column == "uri") {
			ro->set_uri(results[i]);
		} else if (column == "action") {
			ro->set_action(results[i][0]);
		} else {
			return -1;
		}
	}
	ro->set_status(BackupObject::Valid);
	return 0;
}

RemoteObject
RemoteObject::find_by_uri(const std::string& uri)
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

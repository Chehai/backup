#ifndef LOCAL_OBJECT_H
#define LOCAL_OBJECT_H
#include "common.h"
#include "backup_object.h"
#include "remote_store.h"

class LocalObject : public BackupObject {
public:
	LocalObject();
	LocalObject(const boost::filesystem::path&, const boost::filesystem::path&, const std::string&);
	boost::filesystem::path& fs_path();
	int set_fs_path(const boost::filesystem::path&);
	static int sqlite3_find_by_callback(void * data , int count, char ** results, char ** columns);
	static int populate_local_objects_table(sqlite3 *, const boost::filesystem::path&, const std::string&);
	static LocalObject find_by_uri(sqlite3 *, const std::string&);
	static int find_to_put(sqlite3 *, std::list<LocalObject>&);
	static int sqlite3_find_to_put_callback(void * data , int count, char ** results, char ** columns);
	static int new_from_sqlite3(LocalObject& lo, int count, char ** results, char ** columns);
private:
	boost::filesystem::path local_fs_path;
	static std::string find_by_uri_sql;
	static std::string find_by_put_sql;
};
#endif
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
	int insert_to_db();
	int set_fs_path(const boost::filesystem::path&);
	int set_size();
	std::size_t size();
	static int sqlite3_find_by_callback(void * data , int count, char ** results, char ** columns);
	static int populate_local_objects_table(const boost::filesystem::path&, const std::string&);
	static LocalObject find_by_uri(const std::string&);
	static std::list<LocalObject>& find_to_upload();
	static int sqlite3_find_to_upload_callback(void * data , int count, char ** results, char ** columns);
	static int new_from_sqlite3(LocalObject& lo, int count, char ** results, char ** columns);
private:
	boost::filesystem::path local_fs_path;
	static std::list<LocalObject> objects_to_upload;
	size_t local_file_size;
};
#endif
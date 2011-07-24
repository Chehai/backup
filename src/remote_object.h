#ifndef REMOTE_OBJECT_H
#define REMOTE_OBJECT_H
#include "common.h"
#include "backup_object.h"
#include "remote_store.h"
class RemoteStore;
class RemoteObject : public BackupObject {
public:
	RemoteObject(const std::string&, const std::time_t&, char);
	RemoteObject();
	int insert_to_db();
	static int populate_remote_objects_table(RemoteStore *, const boost::filesystem::path&, const std::string&);
	static RemoteObject find_by_uri(const std::string&);
	static int sqlite3_find_by_callback(void * data , int count, char ** results, char ** columns);
	static int new_from_sqlite3(RemoteObject& ro, int count, char ** results, char ** columns);
	static int sqlite3_find_to_delete_callback(void * data , int count, char ** results, char ** columns);
	static int sqlite3_find_to_download_callback(void * data , int count, char ** results, char ** columns);
	static std::list<RemoteObject>& find_to_delete();
	static std::list<RemoteObject>& find_to_download(const std::time_t&);
	char action() const;
	int set_action(char);
private:
	char object_action;
	static std::list<RemoteObject> objects_to_delete;
	static std::list<RemoteObject> objects_to_download;
};
#endif
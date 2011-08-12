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
	static int populate_remote_objects_table(sqlite3 *, RemoteStore *, const boost::filesystem::path&, const std::string&);
	static RemoteObject find_by_uri(sqlite3 *, const std::string&);
	static int sqlite3_find_by_callback(void * data , int count, char ** results, char ** columns);
	static int new_from_sqlite3(RemoteObject& ro, int count, char ** results, char ** columns);
	static int sqlite3_find_to_del_callback(void * data , int count, char ** results, char ** columns);
	static int sqlite3_find_to_get_callback(void * data , int count, char ** results, char ** columns);
	static int find_to_del(sqlite3 *, std::list<RemoteObject>&);
	static int find_to_get(sqlite3 *, const std::time_t, std::list<RemoteObject>&);
	char action() const;
	int set_action(char);
private:
	char object_action;
};
#endif
#ifndef BACKUP_H
#define BACKUP_H
#include "common.h"
#include "remote_store.h"
#include "local_object.h"
class Backup {
public:
	Backup(RemoteStore *, boost::filesystem::path&);
	int backup(const boost::filesystem::path&, const std::string&);
	int restore(const boost::filesystem::path&, const std::string&, const std::time_t&);
private:
	bool dir_ok(const boost::filesystem::path&);
	RemoteStore * remote_store;
	boost::filesystem::path objects_db_path;
};
#endif
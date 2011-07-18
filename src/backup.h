#ifndef BACKUP_H
#define BACKUP_H
#include "local_object.h"
#include "remote_object.h"
#include "remote_store.h"
#include <list>
#include <boost/filesystem.hpp>
class Backup {
public:
	int backup(boost::filesystem::path&, boost::filesystem::path&);
private:
	LocalObject local_object;
	RemoteObject remote_object;
	RemoteStore * remote_store;
	std::list<LocalObject> local_objects;
};
#endif
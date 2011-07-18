#ifndef REMOTE_OBJECT_H
#define REMOTE_OBJECT_H
#include "backup_object.h"
#include "remote_store.h"
#include <boost/filesystem.hpp>
class RemoteStore;
class RemoteObject : public BackupObject {
public:
	RemoteObject(std::string&, Timestamp&);
	RemoteObject();
	static int populate_remote_objects_table(RemoteStore *, boost::filesystem::path&);
private:
};
#endif
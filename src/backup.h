#ifndef BACKUP_H
#define BACKUP_H
#include "common.h"
#include "remote_store.h"
#include "local_object.h"
class Backup {
public:
	int backup(const boost::filesystem::path&, const std::string&);
private:
	RemoteStore * remote_store;
};
#endif
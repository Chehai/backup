#ifndef BACKUP_H
#define BACKUP_H
#include "common.h"
#include "remote_store.h"
class Backup {
public:
	int backup(boost::filesystem::path&, boost::filesystem::path&);
private:
	RemoteStore * remote_store;
};
#endif
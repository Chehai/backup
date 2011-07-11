#ifndef BACKUP_H
#define BACKUP_H
#include "local_object.h"
#include "remote_object.h"
#include "remote_store.h"
class Backup {
public:
	int backup();
private:
	LocalObject local_object;
	RemoteObject remote_object;
	RemoteStore * remote_store;	
};
#endif
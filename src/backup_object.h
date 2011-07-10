#ifndef BACKUP_OBJECT_H
#define BACKUP_OBJECT_H
#include <string>
#include <vector>
#include "backup_store.h"
class BackupObject {
public:
	BackupObject(std::string &);
	BackupObject(const char *);
	BackupObject(char *);
	int backup();
	std::string& local_path();
private:
	std::string object_local_path;
	std::vector<BackupStore *> backup_stores;	
};
#endif
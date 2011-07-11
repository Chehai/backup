#ifndef BACKUP_OBJECT_H
#define BACKUP_OBJECT_H
#include <string>
#include "common.h"

class BackupObject {
public:
	BackupObject();
	BackupObject(std::string&, Timestamp&);
	BackupObject(std::string&);
	BackupObject(const char *, Timestamp&);
	BackupObject(const char *);
	
	std::string& path();
	Timestamp& updated_at();
	virtual ~BackupObject();
private:
	std::string object_path;
	Timestamp object_updated_at;
};
#endif
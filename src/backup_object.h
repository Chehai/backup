#ifndef BACKUP_OBJECT_H
#define BACKUP_OBJECT_H
#include <string>
#include "common.h"

class BackupObject {
public:
	enum Status {
    	Invalid = -1,
		Valid = 0
    };
	BackupObject();
	BackupObject(const std::string&, const Timestamp&);
	BackupObject(const std::string&);
	BackupObject(const char *, const Timestamp&);
	BackupObject(const char *);
	
	const std::string& uri();
	int set_uri(const std::string&);
	const Timestamp& updated_at();
	virtual int set_updated_at();
	int set_updated_at(Timestamp&);
	int set_status(Status);
	Status status();
	virtual ~BackupObject();
protected:
	std::string object_uri;
	Timestamp object_updated_at;
	Status object_status;
};
#endif
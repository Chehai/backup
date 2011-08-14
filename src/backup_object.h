#ifndef BACKUP_OBJECT_H
#define BACKUP_OBJECT_H
#include "common.h"

class BackupObject {
public:
	enum Status {
    	Invalid = -1,
		Valid = 0
    };
	BackupObject();
	BackupObject(const std::string&, const std::time_t&, const std::size_t&);
	BackupObject(const std::string&, const std::time_t&);
	BackupObject(const std::string&);
	BackupObject(const char *, const std::time_t&);
	BackupObject(const char *);
	
	const std::string& uri() const;
	int set_uri(const std::string&);
	int set_uri(const char *);
	const std::time_t& updated_at() const;
	int set_updated_at(const std::time_t&);
	int set_status(Status);
	int set_size(std::size_t);
	std::size_t size();
	Status status();
	virtual ~BackupObject();
protected:
	std::string object_uri;
	std::time_t object_updated_at;
	Status object_status;
	std::size_t object_size;
};
#endif
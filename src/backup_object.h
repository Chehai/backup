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
	BackupObject(const std::string&, const std::time_t&);
	BackupObject(const std::string&);
	BackupObject(const char *, const std::time_t&);
	BackupObject(const char *);
	
	const std::string& uri();
	int set_uri(const std::string&);
	int set_uri(const char *);
	const std::time_t& updated_at();
	int set_updated_at(const std::time_t&);
	int set_status(Status);
	Status status();
	virtual ~BackupObject();
	static int init_db();
	static int close_db();
protected:
	std::string object_uri;
	std::time_t object_updated_at;
	Status object_status;
	static boost::filesystem::path objects_db_path;
	static sqlite3 * objects_db_conn;
};
#endif
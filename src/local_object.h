#ifndef LOCAL_OBJECT_H
#define LOCAL_OBJECT_H
#include <boost/filesystem.hpp>
#include "backup_object.h"
class LocalObject : public BackupObject {
public:
	LocalObject(std::string&);
	LocalObject(const char *);
	
	int set_updated_at();
private:
	boost::filesystem::path filesystem_path;
};
#endif
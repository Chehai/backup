#ifndef LOCAL_OBJECT_H
#define LOCAL_OBJECT_H
#include <boost/filesystem.hpp>
#include "backup_object.h"
class LocalObject : public BackupObject {
public:
	LocalObject(const std::string&, const std::string&);
	LocalObject(const char *, const char *);
	std::string& root();
	int set_updated_at();
private:
	std::string local_root;
};
#endif
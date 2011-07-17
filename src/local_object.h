#ifndef LOCAL_OBJECT_H
#define LOCAL_OBJECT_H
#include <boost/filesystem.hpp>
#include "backup_object.h"
class LocalObject : public BackupObject {
public:
	LocalObject(const boost::filesystem::path&, const std::string&);
	LocalObject(const boost::filesystem::path&, const char *);
	LocalObject(const boost::filesystem::path&);
	boost::filesystem::path& fs_path();
	int set_updated_at();
private:
	boost::filesystem::path local_fs_path;
};
#endif
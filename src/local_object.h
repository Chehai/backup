#ifndef LOCAL_OBJECT_H
#define LOCAL_OBJECT_H
#include "common.h"
#include "backup_object.h"

class LocalObject : public BackupObject {
public:
	LocalObject(const boost::filesystem::path&, const boost::filesystem::path&, const std::string&);
	boost::filesystem::path& fs_path();
	int insert_to_db();
	static int populate_local_objects_table(const boost::filesystem::path&, const std::string&);
private:
	boost::filesystem::path local_fs_path;
};
#endif
#include <iostream>
#include "local_object.h"
LocalObject::LocalObject(std::string& path)
: BackupObject(path)
{
	filesystem_path = path;
}
LocalObject::LocalObject(const char * path)
: BackupObject(path)
{
	filesystem_path = path;
};

int
LocalObject::set_updated_at()
{
	boost::system::error_code err;
	std::time_t t = boost::filesystem::last_write_time(filesystem_path, err);
	if (err.value()) {
		std::cout << "LocalObject::set_updated_at: " << err.message() << std::endl;
		set_status(BackupObject::Invalid);
		return -1;
	}
	object_updated_at = boost::posix_time::from_time_t(t);
	return 0;
}


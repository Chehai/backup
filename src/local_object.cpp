#include <iostream>
#include "local_object.h"
LocalObject::LocalObject(const std::string& local_root_path, const std::string& path)
: BackupObject(path)
{
	local_root = local_root_path;
	set_updated_at();
}
LocalObject::LocalObject(const char * local_root_path, const char * path)
: BackupObject(path)
{
	local_root = local_root_path;
	set_updated_at();
};

std::string&
LocalObject::root()
{
	return local_root;
}
int
LocalObject::set_updated_at()
{
	boost::system::error_code err;
	boost::filesystem::path local_path = local_root;
	boost::filesystem::path name = object_path;
	local_path /= name;
	std::time_t t = boost::filesystem::last_write_time(local_path, err);
	if (err.value()) {
		std::cout << "LocalObject::set_updated_at: " << err.message() << std::endl;
		set_status(BackupObject::Invalid);
		return -1;
	}
	object_updated_at = boost::posix_time::from_time_t(t);
	return 0;
}


#include <iostream>
#include "local_object.h"
LocalObject::LocalObject(const boost::filesystem::path& p, const std::string& uri)
: BackupObject(uri)
{
	local_fs_path = p;
	set_updated_at();
}

LocalObject::LocalObject(const boost::filesystem::path& p, const char * uri)
: BackupObject(uri)
{
	local_fs_path = p;
	set_updated_at();
};

LocalObject::LocalObject(const boost::filesystem::path& p)
: BackupObject(p.filename().string())
{
	local_fs_path = p;
	set_updated_at();
};


boost::filesystem::path&
LocalObject::fs_path()
{
	return local_fs_path;
}
int
LocalObject::set_updated_at()
{
	boost::system::error_code err;
	std::time_t t = boost::filesystem::last_write_time(local_fs_path, err);
	if (err.value()) {
		std::cout << "LocalObject::set_updated_at: " << err.message() << std::endl;
		set_status(BackupObject::Invalid);
		return -1;
	}
	object_updated_at = boost::posix_time::from_time_t(t);
	return 0;
}


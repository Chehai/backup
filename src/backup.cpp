#include "backup.h"

int
Backup::backup(const boost::filesystem::path& backup_dir, const std::string& backup_prefix)
{	
	if (dir_ok(backup_dir)) {
		LocalObject::populate_local_objects_table(backup_dir, backup_prefix); // multi-thread
		RemoteObject::populate_remote_objects_table(remote_store, backup_dir, backup_prefix);
		remote_store->upload(LocalObject::find_to_upload());
		remote_store->unload(RemoteObject::find_to_delete());
	}
	return 0;
}

int 
Backup::restore(const boost::filesystem::path& restore_dir, const std::string& backup_prefix, const std::time_t& timestamp)
{
	if (dir_ok(restore_dir)) {
		RemoteObject::populate_remote_objects_table(remote_store, restore_dir, backup_prefix);
		remote_store->download(RemoteObject::find_to_download(timestamp), restore_dir);
	}
	return 0;
}

bool
Backup::dir_ok(const boost::filesystem::path& dir)
{
	boost::system::error_code err;
	return boost::filesystem::exists(dir, err) && !err.value() && boost::filesystem::is_directory(dir);
}
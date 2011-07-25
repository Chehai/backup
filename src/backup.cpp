#include "backup.h"

Backup::Backup(RemoteStore * rs, boost::filesystem::path& db)
: remote_store(rs), objects_db_path(db)
{
}

int
Backup::backup(const boost::filesystem::path& backup_dir, const std::string& backup_prefix)
{	
	if (dir_ok(backup_dir)) {
		BackupObject::init_db(objects_db_path);
		LocalObject::populate_local_objects_table(backup_dir, backup_prefix); // multi-thread
		RemoteObject::populate_remote_objects_table(remote_store, backup_dir, backup_prefix);
		remote_store->upload(LocalObject::find_to_upload());
		remote_store->unload(RemoteObject::find_to_delete());
		BackupObject::close_db();
	}
	return 0;
}

int 
Backup::restore(const boost::filesystem::path& restore_dir, const std::string& backup_prefix, const std::time_t& timestamp)
{
	if (dir_ok(restore_dir)) {
		BackupObject::init_db(objects_db_path);
		RemoteObject::populate_remote_objects_table(remote_store, restore_dir, backup_prefix);
		remote_store->download(RemoteObject::find_to_download(timestamp), restore_dir);
		BackupObject::close_db();
	}
	return 0;
}

bool
Backup::dir_ok(const boost::filesystem::path& dir)
{
	boost::system::error_code err;
	return boost::filesystem::exists(dir, err) && !err.value() && boost::filesystem::is_directory(dir);
}
#include "backup.h"

int
Backup::backup(const boost::filesystem::path& backup_dir, const std::string& backup_prefix)
{	
	boost::system::error_code err;
	if (boost::filesystem::exists(backup_dir, err)) {
		if (err.value()) {
			return -1;
		}
		if (boost::filesystem::is_directory(backup_dir)) {
			LocalObject::populate_local_objects_table(backup_dir, backup_prefix); // multi-thread
			RemoteObject::populate_remote_objects_table(remote_store, backup_dir, backup_prefix);
			// backup
		} else {
			return -1;
		}
			
	} else {
		return -1;
	}
}
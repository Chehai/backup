#include "remote_object.h"

RemoteObject::RemoteObject(std::string& uri, Timestamp& t)
:BackupObject(uri, t)
{
	
}

RemoteObject::RemoteObject()
{
	
}

int
RemoteObject::populate_remote_objects_table(RemoteStore * remote_store, boost::filesystem::path& backup_dir_prefix)
{
	return 0;
}

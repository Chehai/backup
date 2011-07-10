#include "backup_object.h"

BackupObject::BackupObject(std::string & local_path)
{
	object_local_path = local_path;
}

BackupObject::BackupObject(const char * local_path)
{
	object_local_path = local_path;
}

BackupObject::BackupObject(char * local_path)
{
	object_local_path = local_path;
}

std::string&
BackupObject::local_path()
{
	return object_local_path;
}

int
BackupObject::backup()
{
	return 0;
}
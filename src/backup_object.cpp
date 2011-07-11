#include "backup_object.h"

BackupObject::~BackupObject()
{
}

BackupObject::BackupObject()
{
	object_path = "";
	object_updated_at = now();
}

BackupObject::BackupObject(std::string& path, Timestamp& t)
{
	object_path = path;
	object_updated_at = t;
}

BackupObject::BackupObject(std::string& path)
{
	object_path = path;
	object_updated_at = now();
}


BackupObject::BackupObject(const char * path, Timestamp& t)
{
	object_path = path ? path : "";
	object_updated_at = t;
}

BackupObject::BackupObject(const char * path)
{
	object_path = path ? path : "";
	object_updated_at = now();
}

std::string& 
BackupObject::path()
{
	return object_path;
}

Timestamp&
BackupObject::updated_at()
{
	return object_updated_at;
}

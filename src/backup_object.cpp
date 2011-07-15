#include "backup_object.h"

BackupObject::~BackupObject()
{
}

BackupObject::BackupObject()
{
	object_path = "";
	set_status(BackupObject::Valid);
	set_updated_at();
}

BackupObject::BackupObject(const std::string& path, const Timestamp& t)
{
	object_path = path;
	set_status(BackupObject::Valid);
	object_updated_at = t;
}

BackupObject::BackupObject(const std::string& path)
{
	object_path = path;
	set_status(BackupObject::Valid);
	set_updated_at();
}


BackupObject::BackupObject(const char * path, const Timestamp& t)
{
	object_path = path ? path : "";
	set_status(BackupObject::Valid);
	object_updated_at = t;
}

BackupObject::BackupObject(const char * path)
{
	object_path = path ? path : "";
	set_status(BackupObject::Valid);
	set_updated_at();
}

const std::string& 
BackupObject::path()
{
	return object_path;
}

const Timestamp&
BackupObject::updated_at()
{
	return object_updated_at;
}

int
BackupObject::set_updated_at() 
{
	object_updated_at = now();
	return 0;
}


int 
BackupObject::set_status(BackupObject::Status st)
{
	object_status = st;
	return 0;
}

BackupObject::Status
BackupObject::status()
{
	return object_status;
}


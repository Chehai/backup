#include "backup_object.h"

BackupObject::~BackupObject()
{
}

BackupObject::BackupObject()
{
	object_uri = "";
	set_status(BackupObject::Valid);
	set_updated_at();
}

BackupObject::BackupObject(const std::string& uri, const Timestamp& t)
{
	object_uri = uri;
	set_status(BackupObject::Valid);
	object_updated_at = t;
}

BackupObject::BackupObject(const std::string& uri)
{
	object_uri = uri;
	set_status(BackupObject::Valid);
	set_updated_at();
}


BackupObject::BackupObject(const char * uri, const Timestamp& t)
{
	object_uri = uri ? uri : "";
	set_status(BackupObject::Valid);
	object_updated_at = t;
}

BackupObject::BackupObject(const char * uri)
{
	object_uri = uri ? uri : "";
	set_status(BackupObject::Valid);
	set_updated_at();
}

const std::string& 
BackupObject::uri()
{
	return object_uri;
}

const Timestamp&
BackupObject::updated_at()
{
	return object_updated_at;
}

int
BackupObject::set_updated_at() 
{
	object_updated_at = zero();
	return 0;
}

int
BackupObject::set_updated_at(Timestamp& t)
{
	object_updated_at = t;
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

int
BackupObject::set_uri(const std::string& uri)
{
	object_uri = uri;
	return 0;
}

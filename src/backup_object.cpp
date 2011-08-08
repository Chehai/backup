#include "backup_object.h"

BackupObject::~BackupObject()
{
}

BackupObject::BackupObject()
{
	set_uri("");
	set_status(BackupObject::Valid);
	set_updated_at(std::time_t(0));
}

BackupObject::BackupObject(const std::string& uri, const std::time_t& t)
{
	set_uri(uri);
	set_status(BackupObject::Valid);
	set_updated_at(t);
}

BackupObject::BackupObject(const std::string& uri)
{
	set_uri(uri);
	set_status(BackupObject::Valid);
	set_updated_at(std::time_t(0));
}


BackupObject::BackupObject(const char * uri, const std::time_t& t)
{
	set_uri(uri);
	set_status(BackupObject::Valid);
	set_updated_at(t);
}

BackupObject::BackupObject(const char * uri)
{
	set_uri(uri);
	set_status(BackupObject::Valid);
	set_updated_at(std::time_t(0));
}

const std::string& 
BackupObject::uri() const
{
	return object_uri;
}

const std::time_t&
BackupObject::updated_at() const
{
	return object_updated_at;
}


int
BackupObject::set_updated_at(const std::time_t& t)
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

int
BackupObject::set_uri(const char * uri)
{
	object_uri = uri ? uri : "";
	return 0;
}
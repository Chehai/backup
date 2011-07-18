#include "backup_object.h"
boost::filesystem::path BackupObject::objects_db_path = "objects.db";
sqlite3 * BackupObject::objects_db_conn = NULL;

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

int
BackupObject::init_db()
{
	if (!objects_db_conn) {
		if (sqlite3_open(objects_db_path.c_str(), &objects_db_conn) != SQLITE_OK) {
			close_db();
			return -1;		
		}
	}
	return 0;
}

int
BackupObject::close_db()
{
	if (objects_db_conn) {
		sqlite3_close(objects_db_conn);
		objects_db_conn = NULL;
	}
	return 0;
}

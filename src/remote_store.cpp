#include "remote_store.h"

RemoteStore::RemoteStore()
{
	set_status(RemoteStore::Valid);
}

RemoteStore::~RemoteStore()
{
}

int 
RemoteStore::set_status(RemoteStore::Status st)
{
	store_status = st;
	return 0;
}

RemoteStore::Status
RemoteStore::status()
{
	return store_status;
}

int
RemoteStore::list(const std::string& prefix, std::list<RemoteObject>& remote_objects)
{
	return 0;
}

int
RemoteStore::put(LocalObject& local_object)
{
	return 0;
}

int
RemoteStore::del(RemoteObject& remote_object)
{
	return 0;
}

int
RemoteStore::get(RemoteObject& remote_object, const boost::filesystem::path& dir)
{
	return 0;
}
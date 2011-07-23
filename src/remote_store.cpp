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
RemoteStore::upload(std::list<LocalObject>& local_objects)
{
	for (std::list<LocalObject>::iterator iter = local_objects.begin(); iter != local_objects.end(); ++iter) {
		upload(*iter);
	}
	return 0;
}

int
RemoteStore::upload(LocalObject& local_object)
{
	return 0;
}
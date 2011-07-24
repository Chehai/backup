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

int
RemoteStore::unload(std::list<RemoteObject>& remote_objects)
{
	for (std::list<RemoteObject>::iterator iter = remote_objects.begin(); iter != remote_objects.end(); ++iter) {
		unload(*iter);
	}
	return 0;
}

int
RemoteStore::unload(RemoteObject& remote_object)
{
	return 0;
}

int
RemoteStore::download(std::list<RemoteObject>& remote_objects, const boost::filesystem::path& dir)
{
	for (std::list<RemoteObject>::iterator iter = remote_objects.begin(); iter != remote_objects.end(); ++iter) {
		download(*iter, dir);
	}
	return 0;
}

int
RemoteStore::download(RemoteObject& remote_object, const boost::filesystem::path& dir)
{
	return 0;
}